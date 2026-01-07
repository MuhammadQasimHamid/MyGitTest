#ifndef STAGGINGINDEX_H
#define STAGGINGINDEX_H

#include <iostream>
#include <vector>
#include <filesystem>
#include "utils/fileCRUD.h"
#include "utils/myparser.h"
#include "core/repository.h"
using namespace std::filesystem;
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
    indexEntry()
    {
        mode = hash = offset = path = "";
    }
};

class StagingIndex
{
public:
    static vector<indexEntry> indexEntries;
    static path indexFilePath;
    // StagingIndex(const path &indexFilePath)
    // {
    //     StagingIndex::indexFilePath = indexFilePath;
    //     load();
    // }
    static void InitializeClass(const path &indexFilePath);

    static void addEntry(const indexEntry &entry);

    static void updateEntry(const path &filePath, const string &newHash, const string &newMode);

    static void load();

    static void save();
    static void removeThisFileFromAllIndexEntries(const path &filePath);
    static bool addPathToIndex(const path &dirPath);
    static bool addFileToIndex(const path &filePath);
    static bool addDirectory(const path &dirPath);
    static int removeDeletedWRFilesFromIndex();
    static bool isaConflictFile(const path &filePath);
    static bool isTrackedFile(const path &filePath);
    static bool IndexHasConflicts();
    static indexEntry *getEntry(const path &filePath);

};

#endif