#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include "gitObject.h"
#include "fileCRUD.h"
#include "StagingIndex.h"
using namespace std::filesystem;
using namespace std;
namespace fs = filesystem;

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

    Repository()
    {
    }
    static void InitializeClass();
    
    static bool initRepo(); // returns true if repo initialized successfully
   

    static void storeObject(GitObject gitObj);
    

    static string currentBranch();
    

};

#endif