#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <map>
#include "core/repository.h"
#include "core/gitObject.h"
#include "core/StagingIndex.h"
#include "utils/fileCRUD.h"
#include "dataStructure/Ntree.h"
#include "utils/myparser.h"
#include "utils/mysha1.h"

FileStatus sToFileStatus(string str)
{
    if (str == "same")
        return File_Same;
    if (str == "not exist")
        return File_NotExist;
    if (str == "contents differ")
        return File_ContentsDiffer;
    return File_Nothing;
}
string fileStatusToS(FileStatus fileStatus)
{
    if (fileStatus == File_Same)
        return "same";
    if (fileStatus == File_NotExist)
        return "not exist";
    if (fileStatus == File_ContentsDiffer)
        return "contents differ";
    return "nothing";
}

using namespace std::filesystem;
using namespace std;
namespace fs = filesystem;
#pragma region Repository
fs::path Repository::project_absolute;
fs::path Repository::pitFolderPath;
fs::path Repository::objectsFolderPath;
fs::path Repository::refsFolderPath;
fs::path Repository::refsHeadFolderPath;
fs::path Repository::HEADFilePath;
fs::path Repository::indexFilePath;
fs::path Repository::pitIgnoreFilePath;

void Repository::InitializeClass()
{
    project_absolute = absolute(".");
    pitFolderPath = project_absolute / ".pit";
    objectsFolderPath = pitFolderPath / "objects";
    refsFolderPath = pitFolderPath / "refs";
    refsHeadFolderPath = refsFolderPath / "heads";
    HEADFilePath = pitFolderPath / "HEAD";
    indexFilePath = pitFolderPath / "index";
    pitIgnoreFilePath = project_absolute / ".pitignore";
    StagingIndex::InitializeClass(Repository::indexFilePath);
}
bool Repository::initRepo() // returns true if repo initialized successfully
{
    try
    {
        if (!exists(pitFolderPath))
        {
            // Directories
            create_directory(pitFolderPath);
            // .git/objects
            create_directory(objectsFolderPath);
            // .git/refs
            create_directory(refsFolderPath);
            // .git/refs/heads
            create_directory(refsHeadFolderPath);
            writeFile(refsHeadFolderPath / "master", ""); // main is pointing to empty hash initially
            // Files
            //  .git/HEAD
            // ofstream headFile(HEADFilePath); // this file stores pointer to branches
            // if (headFile.is_open())
            // {
            //     // stores current branch in /HEAD
            //     headFile << ;
            // }
            // headFile.close();
            // .git/StagingIndex::
            writeFile(HEADFilePath, "ref: refs/heads/master");
            // ofstream indexFile(indexFilePath);
            // indexFile.close();
            writeFile(indexFilePath);
        }
        else
        {
            throw runtime_error("Already pit has initialized in current directory.");
        }

        cout << "Initialized empty pit repository." << endl;
        return true;
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
    }
    catch (...)
    {
        cerr << "Their is any error occurs during creating repository." << endl;
    }
    return false;
}
string Repository::storeObject(GitObject gitObj)
{
    string objHash = gitObj.getHash(); //
    path objectDirPath = objectsFolderPath / objHash.substr(0, 2);

    string objectName = objHash.substr(2);
    path objectFilePath = objectDirPath / objectName;
    try
    {
        if (!exists(objectDirPath))
        {

            create_directory(objectDirPath);
            writeFile(objectFilePath, gitObj.serialize());
            cout << "Object Saved:(" << gitObj.getHash() << ")     " << gitObj.serialize();
        }
    }
    catch (const exception &e)
    {
        cout << "Error: " << e.what() << endl;
    }
    catch (...)
    {
        cout << "There is an error in storing object(s)." << endl;
    }
    return objHash;
}
void Repository::generateCommit(string msg)
{
    string treeHash = StoreIndexForCommit(); // TreeHash
    if (treeHash == "")
    {
        cout << "Nothing to commit, working tree clean" << endl;
        return;
    }
    vector<string> parentHashs; // Parent Hashes
    parentHashs.push_back(BranchPointToHashOrNothing(currentBranch()));
    CommitObject CObj(treeHash, parentHashs, "Umar", msg, "12/12/12");
    storeObject(CObj);
    // StagingIndex::indexEntries.clear();
    // StagingIndex::save();
    UpdateBranchHash(currentBranch(), CObj.getHash());
}

string Repository::StoreIndexForCommit()
{
    if (StagingIndex::indexEntries.size() == 0)
    {
        return "";
    }
    NTree tree = NTree();
    for (const auto &iE : StagingIndex::indexEntries)
    {
        tree.root = tree.add(iE);
    }
    string hash = StoreTreeRec(tree.root);
    return hash;
}

string Repository::StoreTreeRec(TreeNode *node)
{
    cout << "Visiting Node: " << node->name << endl;
    // FILE → blob already exists
    if (node->isFile)
    {
        return node->hash;
    }
    TreeObject treeObj;

    for (auto *child : node->children)
    {
        string childHash = StoreTreeRec(child);

        treeEntry entry = treeEntry(child->mode, child->isFile ? Blob : Tree, childHash, child->name);
        entry.name = child->name;
        entry.hash = childHash;

        if (child->isFile)
            entry.mode = "100644";
        else
            entry.mode = "40000";

        treeObj.addEntry(entry);
    }

    // Git requires sorting by name
    sort(treeObj.entires.begin(), treeObj.entires.end(),
         [](const treeEntry &a, const treeEntry &b)
         {
             return a.name < b.name;
         });

    treeObj.serializeContent();
    string treeHash = storeObject(treeObj);
    node->hash = treeHash;
    return treeHash;
}

string Repository::currentBranch()
{
    //     string line;
    //     fstream headFile(HEADFilePath, ios::in); // this file stores pointer to branches
    //     if (headFile.is_open())
    //     {
    //         // stores current branch in /HEAD
    //         getline(headFile, line);
    //         vector<string> parts = split(line, ' ');
    //         path refPath = parts[1];
    //         return refPath.filename().string();
    //     }
    //     return "DETACHED HEAD";

    string line = readFile(HEADFilePath);
    vector<string> parts = split(line, ' ');
    path refPath = parts[1];
    if (refPath == "")
        return "DETACHED HEAD";
    return refPath.filename().string();
}
string Repository::BranchPointToHashOrNothing(string branch)
{
    string branchHashOrNothing = readFile(".pit/refs/heads/master");
    cout << "main:" << branchHashOrNothing << endl;
    return branchHashOrNothing;
}
void Repository::UpdateBranchHash(string branch, string hash)
{
    path branchFile = refsHeadFolderPath / branch;
    cout << "Branch Update: " << (branchFile.string()) << " hash: " << hash;
    writeFile(".pit/refs/heads/master", hash); // change it letter (we will not hardcode)
}
string Repository::getBranchHash(string branch)
{
    path branchFile = refsHeadFolderPath / branch;
    return readFile(branchFile);
}

bool Repository::isInPitIgnore(fs::path pathtoCheck)
{
    string fileContents = readFile(pitIgnoreFilePath);
    vector<string> lines = split(fileContents, '\n');
    for (string l : lines)
    {
            if( l ==  relative(absolute(pathtoCheck), Repository::project_absolute).string())// compare paths correctly || is_a_subfolder(path(l),pathtoCheck))
            return true;
    }
    return false;
}



// Comparisons
FileStatus Repository::IndexWorkingDirComp(indexEntry iE, fs::path filePath)
{

    // .pit/objects/2c/aaef87a6ef8ae6fa87ef6a8e76fa87ef
    fs::path indexFilePath = objectsFolderPath / iE.hash.substr(0, 2) / iE.hash.substr(2);
    if (exists(filePath))
    {
        // if (file_size(filePath) != file_size(indexFilePath) || last_write_time(filePath) != last_write_time(indexFilePath))
        {
            string fileContents = readFile(filePath);
            BlobObject BObj(filePath.filename().string(), fileContents);
            string fileHash = BObj.getHash();
            if (fileHash != iE.hash)
                return File_ContentsDiffer;
        }
        return File_Same;
    }
    else
        return File_NotExist;
}

FileStatus Repository::IndexLastCommitComp(indexEntry iE, string lastCommitFileHash)
{
    if (lastCommitFileHash == "")
        return File_NotExist;
    if (iE.hash == lastCommitFileHash)
        return File_Same;
    return File_ContentsDiffer;
}


map<path,treeEntry> Repository::FlattenTreeObject(TreeObject TObj, path prefix)
{
        map<path,treeEntry> flattenTreeEntries;

        for (auto tE : TObj.entires)
        {
            if (tE.type == Blob)
                flattenTreeEntries[(prefix / tE.name)] = tE;
            else if (tE.type == Tree)
            {
                string rawContents = readFileWithStoredObjectHash(tE.hash);
                TreeObject subTreeObj(rawContents);
                map<path,treeEntry> subFlattenTree = FlattenTreeObject(subTreeObj, prefix / tE.name  );
                for(auto stE: subFlattenTree)
                    flattenTreeEntries[stE.first] = stE.second;
            }
        }
        return flattenTreeEntries;
    }