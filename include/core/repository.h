#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <iostream>
#include <map>
#include <fstream>
#include <filesystem>
#include "core/gitObject.h"
#include "core/StagingIndex.h"
#include "dataStructure/cmpMap.h"
#include "utils/fileCRUD.h"

using namespace std::filesystem;
using namespace std;
namespace fs = filesystem;

struct indexEntry; // to remove the error
enum FileStatus
{
    File_Same,
    File_NotExist,
    File_ContentsDiffer,
    File_Nothing,
};
enum Cmp_Status
{
    CMP_SAME,
    CMP_DIFFER,
    CMP_IN_WR_NotExist_IN,
    CMP_IN_WR_NotExist_WR,
    CMP_IN_C_NotExist_IN,
    CMP_IN_C_NotExist_C,
    CMP_WR_C_NotExist_WR,
    CMP_WR_C_NotExist_C,
};


FileStatus sToFileStatus(string str);
string fileStatusToS(FileStatus fileStatus);

class TreeNode;
class Repository
{
public:
    static path project_absolute;
    static path pitFolderPath;
    static path objectsFolderPath;
    static path refsFolderPath;
    static path refsHeadFolderPath;
    static path HEADFilePath;
    static path indexFilePath;
    static path pitIgnoreFilePath;

    Repository()
    {
    }
    static void InitializeClass();

    static bool initRepo(); // returns true if repo initialized successfully

    static string storeObject(GitObject gitObj);

    static void generateCommit(string msg);

    static string StoreIndexForCommit();

    static string currentBranch();
    static string BranchPointToHashOrNothing(string branch);
    static void UpdateBranchHash(string branch, string hash);
    static string getBranchHash(string branch);
    static string StoreTreeRec(TreeNode *node);

    static bool isInPitIgnore(fs::path pathtoCheck);

    // comparisions
    static Cmp_Status IndexWorkingDirComp(indexEntry iE, fs::path filePath);
    Cmp_Status IndexWorkingDirComp(cmpPair<indexEntry,path> pair);
    
    static Cmp_Status IndexCommitComp(indexEntry iE, string lastCommitFileHash);

    static Cmp_Status WorkingDirCommitComp(path filePath,string commitHash);
    static map<path,treeEntry> FlattenTreeObject(TreeObject TObj, path prefix = path());
    
};

#endif