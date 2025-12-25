#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include "core/repository.h"
#include "core/gitObject.h"
#include "core/StagingIndex.h"
#include "utils/fileCRUD.h"
#include "dataStructure/Ntree.h"
#include "utils/myparser.h"
#include "utils/mysha1.h"

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
            writeFile(HEADFilePath, "ref: refs/heads/master\n");
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
    vector<string> parentHashs;              // Parent Hashes
    parentHashs.push_back(BranchPointToHashOrNothing(currentBranch()));
    CommitObject CObj(treeHash, parentHashs, "Umar", msg, "12/12/12");
    storeObject(CObj);
    UpdateBranchHash(currentBranch(), CObj.getHash());
}

string Repository::StoreIndexForCommit()
{
    // first i will take all entries from staging Index then i will sperate root files and folder from each other
    // then
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
    string branchHashOrNothing = readFile(refsHeadFolderPath / branch);
    cout << "main:" << branchHashOrNothing << endl;
    return branchHashOrNothing;
}
void Repository::UpdateBranchHash(string branch, string hash)
{
    writeFile(refsHeadFolderPath / branch, hash);
}

bool Repository::isInPitIgnore(fs::path pathtoCheck)
{
    string fileContents = readFile(pitIgnoreFilePath);
    vector<string> lines = split(fileContents, '\n');
    for (string l : lines)
    {
        if (l == pathtoCheck.filename().string())
            return true;
    }
    return false;
}
