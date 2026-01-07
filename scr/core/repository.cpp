#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <map>
#include <queue>
#include <unordered_set>
#include "utils/mytime.h"
#include "core/repository.h"
#include "core/gitObject.h"
#include "core/StagingIndex.h"
#include "core/UserConfig.h"
#include "utils/fileCRUD.h"
#include "dataStructure/Ntree.h"
#include "utils/myparser.h"
#include "commands/checkout.h"
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
    // project_absolute = absolute(".");
    project_absolute = "D:/3rd Sems/DSA/DSAL/VersioningTestUsingPit";
    pitFolderPath = project_absolute / ".pit";
    objectsFolderPath = pitFolderPath / "objects";
    refsFolderPath = pitFolderPath / "refs";
    refsHeadFolderPath = refsFolderPath / "heads";
    HEADFilePath = pitFolderPath / "HEAD";
    indexFilePath = pitFolderPath / "index";
    pitIgnoreFilePath = project_absolute / ".pitignore";
    StagingIndex::InitializeClass(Repository::indexFilePath);
    UserConfig::InitializeClass();
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

            UserConfig::InitLocalConfigFile();
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
    // cout << "going to store object with hash: " << objHash << endl;
    try
    {
        if (!exists(objectDirPath))
        {

            create_directory(objectDirPath);
        }
        writeFile(objectFilePath, gitObj.serialize());
        cout << "Object Saved:(" << gitObj.getHash() << ")     " << gitObj.serialize();
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
    vector<string> parentHashs; // Parent Hashes
    parentHashs.push_back(BranchPointToHashOrNothing(currentBranch()));
    if (Repository::isMergingInProgress())
    {
        if (StagingIndex::IndexHasConflicts())
        {
            cout << "Cannot commit until all merge conflicts are resolved." << endl;
            return;
        }
        cout << "Merging in progress. Completing merge commit." << endl;

        path mergeMsgFile = Repository::pitFolderPath / "MERGE_MSG";
        path mergeHeadFile = Repository::pitFolderPath / "MERGE_HEAD";
        string mergeCommitHash = readFile(mergeHeadFile);
        parentHashs.push_back(mergeCommitHash);
        msg = "Merge commit: " + msg;
        deleteFile(mergeHeadFile);
        deleteFile(mergeMsgFile);

        // return;
    }
    string treeHash = StoreIndexForCommit(); // TreeHash
    if (treeHash == "")
    {
        cout << "Nothing to commit, working tree clean" << endl;
        return;
    }
    string author = UserConfig::getName() + "<" + UserConfig::getEmail() + ">";
    CommitObject CObj(treeHash, parentHashs, author, msg, epochToString(getEpochSeconds()));
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
bool Repository::isMergingInProgress()
{
    path mergeHeadFile = Repository::pitFolderPath / "MERGE_HEAD";
    return exists(mergeHeadFile);
}
void Repository::mergeBranch(string branchToMerge)
{
    string cBranch = currentBranch();
    if (branchToMerge == cBranch)
    {
        cout << "Cannot merge a branch into itself." << endl;
        return;
    }
    if (applyFastForwardMerge(cBranch, branchToMerge))
    {
        cout << "Fast Forward applied" << endl;
        char *br = const_cast<char *>(cBranch.c_str());
        char *args[] = {const_cast<char *>("pit"), const_cast<char *>("checkout"), br};
        checkoutCommandExe(3, args);
    }
    else
    {
        if (applyTreeWayMerge(cBranch, branchToMerge))
        {
            cout << "3-Way Merge Applied Seccessfully" << endl;
            char *br = const_cast<char *>(cBranch.c_str());
            char *args[] = {const_cast<char *>("pit"), const_cast<char *>("checkout"), br};
            checkoutCommandExe(3, args);
        }
    }
}

bool Repository::applyTreeWayMerge(string cBranch, string branchToMerge)
{
    bool anyConflictFoundInMerging = false;
    string branchToMergeHash = Repository::BranchPointToHashOrNothing(branchToMerge);
    string cBranchHash = Repository::BranchPointToHashOrNothing(cBranch);
    string ancestorCommitHash = getCommanAncestorCommit(cBranchHash, branchToMergeHash);
    CommitObject ancestorCommitObj(readFileWithStoredObjectHash(ancestorCommitHash));
    CommitObject cBranchCommitObj(readFileWithStoredObjectHash(cBranchHash));
    CommitObject branchToMergeCommitObj(readFileWithStoredObjectHash(branchToMergeHash));
    TreeObject ancestorTreeObj(readFileWithStoredObjectHash(ancestorCommitObj.treeHash));
    TreeObject cBranchTreeObj(readFileWithStoredObjectHash(cBranchCommitObj.treeHash));
    TreeObject branchToMergeTreeObj(readFileWithStoredObjectHash(branchToMergeCommitObj.treeHash));
    map<path, treeEntry> ancestorFlattenTree = Repository::FlattenTreeObject(ancestorTreeObj);
    map<path, treeEntry> cBranchFlattenTree = Repository::FlattenTreeObject(cBranchTreeObj);
    map<path, treeEntry> branchToMergeFlattenTree = Repository::FlattenTreeObject(branchToMergeTreeObj);
    cmpThWayMap<path, treeEntry, treeEntry, treeEntry> cmpMap3Way;
    TreeObject mergedTreeObj;
    StagingIndex::indexEntries.clear(); // clear index for new merge commit
    StagingIndex::save();

    for (auto it : ancestorFlattenTree)
    {
        treeEntry tE = it.second;
        cmpMap3Way.addVal1(it.first, tE);
    }
    for (auto it : cBranchFlattenTree)
    {
        treeEntry tE = it.second;
        cmpMap3Way.addVal2(it.first, tE);
    }
    for (auto it : branchToMergeFlattenTree)
    {
        treeEntry tE = it.second;
        cmpMap3Way.addVal3(it.first, tE);
    }
    // cout << "3 Way Comparison Table: " << endl;
    // for (auto it : cmpMap3Way.umap)
    // {
    //     cout << it.first << " : ";
    //     cout << " Val1Exists:(ancestor) " << it.second.val1Exists() << " , ";
    //     cout << " Val2Exists:(cBranch) " << it.second.val2Exists() << " , ";
    //     cout << " Val3Exists:(branchToMerge) " << it.second.val3Exists() << endl;
    // }
    for (auto it : cmpMap3Way.umap)
    {
        const path &filePath = it.first;
        cout << filePath;
        treeEntry ancestorTE = it.second.getVal1();
        treeEntry cBranchTE = it.second.getVal2();
        treeEntry branchToMergeTE = it.second.getVal3();
        treeEntry tE;
        bool addToTree = false;
        bool hasConflict = false;
        if (it.second.val1Exists() && it.second.val2Exists() && it.second.val3Exists())
        {
            // present in all three
            if (ancestorTE.hash == cBranchTE.hash && ancestorTE.hash == branchToMergeTE.hash)
            {
                // same in all three
                tE = ancestorTE;
                addToTree = true;
                cout << "  same in all three" << endl;
            }
            else if (ancestorTE.hash == cBranchTE.hash && ancestorTE.hash != branchToMergeTE.hash)
            {
                // changed in branchToMerge only
                tE = branchToMergeTE;
                addToTree = true;
                cout << " changed in branchToMerge only" << endl;
            }
            else if (ancestorTE.hash != cBranchTE.hash && ancestorTE.hash == branchToMergeTE.hash)
            {
                // changed in cBranch only
                tE = cBranchTE;
                addToTree = true;
                cout << " changed in cBranch only" << endl;
            }
            else
            {
                // changed in both - conflict
                cout << " changed in both - CONFLICT" << endl;
                indexEntry iE1(ancestorTE.mode, ancestorTE.hash, "1", filePath.string());
                indexEntry iE2(cBranchTE.mode, cBranchTE.hash, "2", filePath.string());
                indexEntry iE3(branchToMergeTE.mode, branchToMergeTE.hash, "3", filePath.string());
                StagingIndex::addEntry(iE1); // add to index also
                StagingIndex::addEntry(iE2);
                StagingIndex::addEntry(iE3);
                hasConflict = true;
                addToTree = true;
            }
        }
        else if (it.second.val1Exists() && !it.second.val2Exists() && !it.second.val3Exists())
        {
            // deleted in both cBranch and branchToMerge
            // tE
            addToTree = false;
            cout << " deleted in both cBranch and branchToMerge" << endl;
        }
        else if (it.second.val1Exists() && it.second.val2Exists() && !it.second.val3Exists())
        {
            // deleted in branchToMerge (Theirs)
            tE = cBranchTE;
            addToTree = true;
            cout << " deleted in branchToMerge" << endl;
        }
        else if (it.second.val1Exists() && !it.second.val2Exists() && it.second.val3Exists())
        {
            // deleted in cBranch (Ours) so keep deletion
            // tE
            addToTree = false;
            cout << " deleted in cBranch" << endl;
        }
        else if (!it.second.val1Exists())
        {
            if (it.second.val2Exists() && it.second.val3Exists())
            {
                if (cBranchTE.hash == branchToMergeTE.hash)
                {
                    tE = cBranchTE;
                    addToTree = true;
                    cout << " added in both branches with same content" << endl;
                }
                else
                {
                    addToTree = true;
                    hasConflict = true;
                    cout << "conflict detected " << endl;
                    indexEntry iE2(cBranchTE.mode, cBranchTE.hash, "2", filePath.string());
                    indexEntry iE3(branchToMergeTE.mode, branchToMergeTE.hash, "3", filePath.string());
                    StagingIndex::addEntry(iE2);
                    StagingIndex::addEntry(iE3);
                    // confict
                    //  indexEntry iE(tE.mode, tE.hash, "0", filePath.string());
                    //  mergeCommittree.add(iE);
                    //  tE = branchToMergeTE;
                }
            }
            else if (it.second.val2Exists())
            {
                addToTree = true;
                tE = cBranchTE;
            }
            else if (it.second.val3Exists())
            {
                addToTree = true;
                tE = branchToMergeTE;
            }
        }

        if (addToTree)
        {
            if (hasConflict)
            {

                string cBranchFileContents = readFileWithStoredObjectHash(cBranchTE.hash);
                string branchToMergeFileContents = readFileWithStoredObjectHash(branchToMergeTE.hash);

                BlobObject cBranchBlob = BlobObject(cBranchFileContents);
                BlobObject branchToMergeBlob = BlobObject(branchToMergeFileContents);

                string conflict =
                    "<<<<<<< HEAD\n" +
                    cBranchBlob.contents +
                    "\n=======\n" +
                    branchToMergeBlob.contents +
                    "\n>>>>>>> " + branchToMerge + "\n";
                BlobObject conflictBlob(branchToMergeTE.name, conflict);
                string conflictBlobHash = Repository::storeObject(conflictBlob);
                cout <<  "Conflict Blob Stored with hash: " << conflictBlobHash << endl;
                writeFile(filePath, conflictBlob.contents); // write conflict to working directory
                anyConflictFoundInMerging = true;
            }
            else
            {
                indexEntry iE(tE.mode, tE.hash, "0", filePath.string());
                StagingIndex::addEntry(iE); // add to index also
                BlobObject bObj(readFileWithStoredObjectHash(tE.hash));
                writeFile(filePath, bObj.contents); // write to working directory
            }
        }
    }
    StagingIndex::save(); // save index after merge (may or may not have conflicts)
    if (anyConflictFoundInMerging)
    {
        cout << "Merge completed with conflicts. Please resolve them manually." << endl;
        path mergeHeadFile = Repository::pitFolderPath / "MERGE_HEAD";
        writeFile(mergeHeadFile, branchToMergeHash);
        path mergeMsgFile = Repository::pitFolderPath / "MERGE_MSG";
        writeFile(mergeMsgFile, branchToMerge + " merged into " + cBranch);
        return false;
    }

    generateCommit(branchToMerge + " merged into " + cBranch);
    // string treeHash = StoreTreeRec(mergeCommittree.root);
    // if (treeHash == "")
    // {
    //     cout << "Nothing to commit, working tree clean" << endl;
    //     // return;
    // }
    // vector<string> parentHashs; // Parent Hashes
    // parentHashs.push_back(cBranchHash);
    // parentHashs.push_back(branchToMergeHash);
    // string author = UserConfig::getName() + "<" + UserConfig::getEmail() + ">";
    // string msg = branchToMerge + " merged into " + cBranch;
    // CommitObject CObj(treeHash, parentHashs, author, msg, epochToString(getEpochSeconds()));
    // storeObject(CObj);
    return true;
}
string Repository::getCommanAncestorCommit(string a, string b)
{
    unordered_set<string> ancestors;

    // Collect all ancestors of A
    queue<string> q;
    q.push(a);
    while (!q.empty())
    {
        string c = q.front();
        q.pop();
        if (ancestors.insert(c).second)
        {
            CommitObject cObj(readFileWithStoredObjectHash(c));
            for (auto &p : cObj.parentHash)
                if (p != "")
                    q.push(p);
        }
    }

    // Walk B until we hit an ancestor
    q.push(b);
    while (!q.empty())
    {
        string c = q.front();
        q.pop();
        if (ancestors.count(c))
            return c;
        CommitObject cObj(readFileWithStoredObjectHash(c));
        for (auto &p : cObj.parentHash)
            if (p != "")
                q.push(p);
    }

    return ""; // should not happen
}
bool Repository::applyFastForwardMerge(string baseBranch, string branchToMerge)
{
    string baseBranchHash = Repository::BranchPointToHashOrNothing(baseBranch);
    string mergeToBranchHash = Repository::BranchPointToHashOrNothing(branchToMerge);
    // cout << "base: " << baseBranchHash << endl;
    // cout << "mergeToBranch: " << mergeToBranchHash << endl;
    string tempHash = mergeToBranchHash;
    while (tempHash != "")
    {
        // cout << "Traversal" << tempHash << endl;
        // system("pause");
        if (tempHash == baseBranchHash) // perform merge
        {
            cout << "Fast Forward Merge possible" << endl;
            UpdateBranchHash(baseBranch, mergeToBranchHash);
            return true;
        }
        string mergeBrFileContents = readFileWithStoredObjectHash(tempHash);
        CommitObject mergeBrObj(mergeBrFileContents);
        // cout << "Parent  " << mergeBrObj.parentHash[0] << endl;
        tempHash = mergeBrObj.parentHash[0];
    }
    cout << "Fast Forward Merge not possible" << endl;
    return false;
}

string Repository::StoreTreeRec(TreeNode *node)
{
    // cout << "Visiting Node: " << node->name << endl;
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
        // entry.name = child->name;
        // entry.hash = childHash;

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
bool Repository::isaBranch(string branchToCheck)
{
    vector<string> allBranches = getAllBranches();
    for (auto br : allBranches)
        if (br == branchToCheck)
            return true;
    return false;
}
string Repository::currentBranch()
{
    string line = readFile(HEADFilePath);
    vector<string> parts = split(line, ' ');
    path refPath = parts[1];
    if (refPath == "")
        return "DETACHED HEAD";
    return refPath.filename().string();
}
string Repository::BranchPointToHashOrNothing(string branch)
{
    string branchHashOrNothing = readFile(Repository::refsHeadFolderPath / branch);
    // cout << "main:" << branchHashOrNothing << endl;
    return branchHashOrNothing;
}
void Repository::UpdateBranchHash(string branch, string hash)
{
    path branchFile = refsHeadFolderPath / branch;
    // cout << "Branch Update: " << (branchFile.string()) << " hash: " << hash;
    writeFile(Repository::refsHeadFolderPath / branch, hash); // change it letter (we will not hardcode)
}
string Repository::getBranchHash(string branch)
{
    path branchFile = refsHeadFolderPath / branch;
    return readFile(branchFile);
}
vector<string> Repository::getAllBranches()
{
    vector<string> res;
    for (const auto &entry : fs::directory_iterator(Repository::refsHeadFolderPath))
    {
        res.push_back(entry.path().filename().string());
    }
    return res;
}
void Repository::setHEAD(string branchOrHash)
{
    if (branchOrHash.length() == 40 && storedObjectExistsWithHash(branchOrHash))
        writeFile(HEADFilePath, "hash: " + branchOrHash);
    else
        writeFile(HEADFilePath, "ref: refs/heads/" + branchOrHash);
}
pair<string, string> Repository::getHEAD()
{
    string line = readFile(HEADFilePath);
    vector<string> parts = split(line, ' ');
    if (parts.size() < 2)
        return make_pair("", "");
    return make_pair(parts[0], parts[1]);
}

// bool Repository::isInPitIgnore(fs::path pathtoCheck)
// {
//     string fileContents = readFile(pitIgnoreFilePath);
//     vector<string> lines = split(fileContents, '\n');
//     for (string l : lines)
//     {
//             if( l ==  relative(absolute(pathtoCheck), Repository::project_absolute).string() || is_a_subfolder(path(l),pathtoCheck))
//             return true;
//     }
//     return false;
// }

bool Repository::stagedChangesExist()
{
    string cBranch = currentBranch();
    string cBranchHash = getBranchHash(cBranch);
    map<path, treeEntry> flattenTree;
    cmpMap<path, indexEntry, treeEntry> cmpMapiEtE;
    // fill the flatten tree
    for (auto iE : StagingIndex::indexEntries)
        cmpMapiEtE.addVal1(iE.path, iE);
    if (cBranchHash != "")
    {
        string rawFileContentsCommit = readFileWithStoredObjectHash(cBranchHash);
        CommitObject CObj(rawFileContentsCommit);
        string rawFileContentsTree = readFileWithStoredObjectHash(CObj.treeHash);
        TreeObject TObj(rawFileContentsTree);
        flattenTree = FlattenTreeObject(TObj);
        // key value1 value2
        for (auto keyValuePair : flattenTree)
            cmpMapiEtE.addVal2(keyValuePair.first, keyValuePair.second);
    }

    // cout << "Comparing with Last Commit: " << StagingIndex::indexEntries.size() <<"" << endl;
    for (auto cmpRow : cmpMapiEtE.umap)
    {
        cmpPair iEtEPair = cmpRow.second;
        Cmp_Status cmpStatus = IndexCommitComp(iEtEPair.getVal1(), iEtEPair.getVal2().hash);
        if (cmpStatus == CMP_DIFFER || !iEtEPair.val2Exists() || !iEtEPair.val1Exists())
        {
            return true;
        }
    }
    return false;
}

bool Repository::unstagedChangesExist()
{
    for (auto iE : StagingIndex::indexEntries)
    {
        Cmp_Status cmpStatus = IndexWorkingDirComp(iE, iE.path);
        if (cmpStatus == CMP_DIFFER || cmpStatus == CMP_IN_WR_NotExist_WR)
            return true;
    }
    return false;
}

bool Repository::isInPitIgnore(fs::path pathToCheck)
{
    fs::path relPath = normalize_relative(pathToCheck);

    // Read the ignore file once
    std::string fileContents = readFile(pitIgnoreFilePath);
    std::vector<std::string> lines = split(fileContents, '\n');

    for (std::string rule : lines)
    {
        // Skip empty lines
        if (rule.empty())
            continue;

        // Normalize rule
        fs::path rulePath = fs::path(rule).lexically_normal();

        // Directory ignore (ends with '/')
        if (rule.back() == '/')
        {
            // remove trailing '/' for comparison
            rulePath = rulePath.parent_path() / rulePath.filename();
            if (is_subpath(rulePath, relPath))
                return true;
        }
        else
        {
            // Exact file or directory match
            if (relPath == rulePath || relPath.filename() == rulePath.filename())
                return true;
        }
    }

    return false;

    // fs::path relPath = normalize_relative(pathToCheck);

    // string fileContents = readFile(pitIgnoreFilePath);
    // vector<string> lines = split(fileContents, '\n');

    // for (string rule : lines)
    // {
    //     if (rule.empty())
    //         continue;

    //     fs::path rulePath = fs::path(rule).lexically_normal();

    //     // 1. Exact file name match (e.g. test.exe)
    //     if (relPath.filename() == rulePath)
    //         return true;

    //     // 2. Exact path match
    //     if (relPath == rulePath)
    //         return true;

    //     // 3. Directory ignore (build/, .git/, etc.)
    //     if (is_subpath(rulePath, relPath))
    //         return true;
    // }

    // return false;
}

// Comparisons
// CMP_Same
// CMP_Differ
// CMP_IN_WR_NotExist_IN
// CMP_IN_WR_NotExist_WR
// CMP_IN_H_NotExist_IN
// CMP_IN_H_NotExist_H
// CMP_WR_H_NotExist_WR
// CMP_WR_H_NotExist_H

Cmp_Status Repository::IndexWorkingDirComp(indexEntry iE, fs::path filePath)
{

    if (iE.hash == "")
        return CMP_IN_WR_NotExist_IN;
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
                return CMP_DIFFER;
        }
        return CMP_SAME;
    }
    else
        return CMP_IN_WR_NotExist_WR;
}

Cmp_Status Repository::IndexCommitComp(indexEntry iE, string lastCommitFileHash)
{
    if (iE.hash == "")
        return CMP_IN_C_NotExist_C;
    if (lastCommitFileHash == "")
        return CMP_IN_C_NotExist_IN;
    if (iE.hash == lastCommitFileHash)
        return CMP_SAME;
    return CMP_DIFFER;
}
Cmp_Status Repository::WorkingDirCommitComp(path filePath, string commitTreeFileHash)
{
    if (commitTreeFileHash == "")
        return CMP_WR_C_NotExist_C;
    try
    {
        string rawFile = readFile(filePath);
        BlobObject bObj(filePath.filename().string(), rawFile); // calling normal construction DD
        string hash = bObj.getHash();
        if (hash == commitTreeFileHash)
            return CMP_SAME;
        else
            return CMP_DIFFER;
    }
    catch (...)
    {
        return CMP_WR_C_NotExist_WR;
    }
}

// overloaded
Cmp_Status Repository::IndexWorkingDirComp(cmpPair<indexEntry, path> pair)
{

    if (!pair.val1Exists())
        return CMP_IN_WR_NotExist_IN;
    // .pit/objects/2c/aaef87a6ef8ae6fa87ef6a8e76fa87ef
    fs::path indexFilePath = objectsFolderPath / pair.getVal1().hash.substr(0, 2) / pair.getVal1().hash.substr(2);
    if (exists(pair.getVal2()))
    {
        // if (file_size(filePath) != file_size(indexFilePath) || last_write_time(filePath) != last_write_time(indexFilePath))
        {
            string fileContents = readFile(pair.getVal2());
            BlobObject BObj(pair.getVal2().filename().string(), fileContents);
            string fileHash = BObj.getHash();
            if (fileHash != pair.getVal1().hash)
                return CMP_DIFFER;
        }
        return CMP_SAME;
    }
    else
        return CMP_IN_WR_NotExist_WR;
}

map<path, treeEntry> Repository::FlattenTreeObject(TreeObject TObj, path prefix)
{
    map<path, treeEntry> flattenTreeEntries;

    for (auto tE : TObj.entires)
    {
        if (tE.type == Blob)
            flattenTreeEntries[(prefix / tE.name)] = tE;
        else if (tE.type == Tree)
        {
            string rawContents = readFileWithStoredObjectHash(tE.hash);
            TreeObject subTreeObj(rawContents);
            map<path, treeEntry> subFlattenTree = FlattenTreeObject(subTreeObj, prefix / tE.name);
            for (auto stE : subFlattenTree)
                flattenTreeEntries[stE.first] = stE.second;
        }
    }
    return flattenTreeEntries;
}