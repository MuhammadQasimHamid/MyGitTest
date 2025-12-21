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


void Repository::InitializeClass()
{
    project_absolute = absolute(".");
    pitFolderPath = project_absolute / ".pit";
    objectsFolderPath = pitFolderPath / "objects";
    refsFolderPath = pitFolderPath / "refs";
    refsHeadFolderPath = refsFolderPath / "heads";
    HEADFilePath = pitFolderPath / "HEAD";
    indexFilePath = pitFolderPath / "StagingIndex::";
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

            // Files
            //  .git/HEAD
            ofstream headFile(HEADFilePath); // this file stores pointer to branches
            if (headFile.is_open())
            {
                // stores current branch in /HEAD
                headFile << "ref: refs/heads/main\n";
            }
            headFile.close();
            // .git/StagingIndex::
            ofstream indexFile(indexFilePath);
            indexFile.close();
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
            writeFile(objectFilePath, gitObj.contents);
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

string Repository::currentBranch()
{
    string line;
    fstream headFile(HEADFilePath, ios::in); // this file stores pointer to branches
    if (headFile.is_open())
    {
        // stores current branch in /HEAD
        getline(headFile, line);
        vector<string> parts = split(line, ' ');
        path refPath = parts[1];
        return refPath.filename().string();
    }
    return "DETACHED HEAD";
}
