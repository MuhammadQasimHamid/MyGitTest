#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include "gitObject.cpp"
#include "fileCRUD.cpp"
using namespace std::filesystem;
using namespace std;
namespace fs = filesystem;

struct indexEntry
{
    string mode;
    string hash;
    string offset;
    string path;
    indexEntry(string mode, string hash, string offset, string path)
    {
        this->mode = mode;
        this->hash = hash;
        this->offset = offset;
        this->path = path;
    }
    void fill(string &mode, string &hash, string &offset, string &path)
    {
        mode = this->mode;
        hash = this->hash;
        offset = this->offset;
        path = this->path;
    }
};

class Repository
{
public:
    path project_absolute = absolute(".");
    path pitFolderPath = project_absolute / ".pit";
    path objectsFolderPath = pitFolderPath / "objects";
    path refsFolderPath = pitFolderPath / "refs";
    path refsHeadFolderPath = refsFolderPath / "heads";
    path HEADFilePath = pitFolderPath / "HEAD";
    path indexFilePath = pitFolderPath / "index";

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

    void storeObject(GitObject gitObj)
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

    string currentBranch()
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

    bool addFileToIndex(path filePath)
    {
        vector<indexEntry> indexEntries = loadIndexEntries();
        for (auto iE : indexEntries)
        {
            if (iE.path == filePath)
            {
                // cout << "File Already Added" << endl;
                return false;
            }
        }
        string fileContents = readFile(filePath);
        if (!exists(filePath))
        {
            cout << "File not exists" << endl;
            return false;
        }
        BlobObject B(filePath.filename().string(), fileContents);
        indexEntry iE("100644", "hash", "0", filePath.string());
        indexEntries.push_back(iE);
        // cout << "index entries" << indexEntries.size() << endl;
        saveIndexEntries(indexEntries);
        return true;
    }
    vector<indexEntry> loadIndexEntries()
    {
        string indexFileContents = readFile(indexFilePath);
        cout << "readed: " << indexFileContents;
        vector<string> lines = split(indexFileContents, '\n');
        vector<indexEntry> indexEntries;
        for (auto l : lines)
        {
            vector<string> parts = split(l, ' ');
            if (parts.size() == 4)
            {
                indexEntry iE(parts[0], parts[1], parts[2], parts[3]);
                indexEntries.push_back(iE);
            }
        }
        // cout << "index entries" << indexEntries.size() << endl;
        return indexEntries;
    }
    void saveIndexEntries(vector<indexEntry> indexEntires)
    {
        string mode, hash, offset, filepath;
        string indexFileContents = "";
        for (auto iE : indexEntires)
        {
            iE.fill(mode, hash, offset, filepath);
            indexFileContents += mode + " " + hash + " " + offset + " " + filepath + "\n";
        }
        writeFile(indexFilePath, indexFileContents);
    }
};

#endif