#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include "gitObject.cpp"
using namespace std::filesystem;
#include "gitObject.cpp"
using namespace std;
namespace fs = filesystem;
class Repository
{
public:
    fs::path project_absolute = fs::absolute(".");
    fs::path pitFolderPath = project_absolute / ".pit";
    fs::path objectsFolderPath = pitFolderPath / "objects";
    fs::path refsFolderPath = pitFolderPath / "refs";
    fs::path refsHeadFolderPath = refsFolderPath / "heads";
    fs::path HEADFilePath = pitFolderPath / "HEAD";
    fs::path indexFilePath = pitFolderPath / "index";

    Repository()
    {
    }
    bool initRepo() // returns true if repo initialized successfully
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
                // .git/index
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

    void storeObject(GitObject object) void storeObject(GitObject gitObj)
    {
        string hash = object.getHash();
        // path dirName = objectsFolderPath /
    }
};
string objHash = gitObj.getHash(); //
fs::path objectDirPath = objectsFolderPath / objHash.substr(0, 2);

string objectName = objHash.substr(2);
fs::path objectFilePath = objectDirPath / objectName;
try
{
    if (!exists(objectDirPath))
    {
        fs::create_directory(objectDirPath);
        ofstream objectFile(objectFilePath);
        if (objectFile.is_open())
        {
            objectFile << gitObj.contents;
        }
        objectFile.close();
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
string currentBranch()
{
    string line;
    fstream headFile(HEADFilePath, ios::in); // this file stores pointer to branches
    if (headFile.is_open())
    {
        // stores current branch in /HEAD
        getline(headFile, line);
        vector<string> parts = split(line, ' ');
        fs::path refPath = parts[1];
        return refPath.filename().string();
    }
    return "DETACHED HEAD";
}

void addFileToIndex(fs::path filePath)
{
}
}
;

#endif