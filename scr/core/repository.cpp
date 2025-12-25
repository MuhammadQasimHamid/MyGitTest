#include "core/repository.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include "core/gitObject.h"
#include "core/StagingIndex.h"
#include "utils/fileCRUD.h"
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
void Repository::storeObject(GitObject gitObj)
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
}
void Repository::generateCommit(string msg)
{
    TreeObject treeObj = StoreDirTree(project_absolute); // TreeHash
    string treeHash = treeObj.getHash();
    vector<string> parentHashs; // Parent Hashes
    parentHashs.push_back(BranchPointToHashOrNothing(currentBranch()));
    CommitObject CObj(treeHash, parentHashs, "Qasim", msg, "12/12/12");
    storeObject(CObj);
    UpdateBranchHash(currentBranch(), CObj.getHash());
}

TreeObject Repository::StoreDirTree(fs::path dirPath)
{
    TreeObject treeObject; // no need to write treeObject() instead of treeObject
    for (auto &dirEntry : fs::directory_iterator(dirPath))
    {
        cout << "Path :" << dirEntry.path().relative_path();

        // string name = dirEntry.path().filename().string();
        if (!isInPitIgnore(dirEntry.path()))
        {
            if (fs::is_regular_file(dirEntry.path()))
            {
                cout << "      is a file and ";
                if (StagingIndex::isTrackedFile(dirEntry.path().relative_path())) // if this file is a tracked file/ means available in staggingArea/index
                {
                    cout << "     is in tracking";
                    indexEntry *te = StagingIndex::getEntry(dirEntry.path());                        // get the entry having this file
                    treeEntry tEntry(te->mode, Blob, te->hash, dirEntry.path().filename().string()); // make treeEntry to push into treeObject
                    // storeObject() is not needed here bcz this is a tracking file and tracked/staged files are already stored in object folder
                    treeObject.addEntry(tEntry);
                }
            }
            else if (fs::is_directory(dirEntry.path()))
            {
                cout << "      is a directory";
                TreeObject subtreeObj = StoreDirTree(dirEntry.path());
                string subtreeHash = subtreeObj.getHash();
                cout << "subtree Contains " << subtreeObj.entires.size() << endl;
                if (subtreeObj.entires.size() > 0) // if there exists entires in tree
                {
                    storeObject(subtreeObj); // store this tree object
                    // add this subtreehash into mainTreeEntries
                    treeEntry tEntry("10006", Tree, subtreeHash, dirEntry.path().filename().string()); // make treeEntry to push into treeObject
                    treeObject.addEntry(tEntry);
                    cout << "   subtree saved";
                }
                else
                {
                    cout << "   subtree ignored as it has no entry";
                }
            }
        }
        else
        {
            cout << "    ignore it";
        }
        cout << endl;
    }
    return treeObject;
}

TreeObject Repository::StoreIndexForCommit()
{
    // first i will take all entries from staging Index then i will sperate root files and folder from each other
    // then

    TreeObject treeObject;
    return treeObject;
}

TreeObject Repository::StoreTreeRec(string prefix) // dir is relative path
{
    TreeObject treeObject;
    int size = prefix.length();
    for (const auto iE : StagingIndex::indexEntries)
    {
        if (iE.path.substr(0, size - 1) == prefix)
        {
        }
    }

    return treeObject;
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
