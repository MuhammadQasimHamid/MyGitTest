#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include "core/gitObject.h"
#include "core/StagingIndex.h"
#include "utils/fileCRUD.h"
using namespace std::filesystem;
using namespace std;
namespace fs = filesystem;

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
    static string StoreTreeRec(TreeNode *node);

    static bool isInPitIgnore(fs::path pathtoCheck);
};

#endif