#include "StagingIndex.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include "fileCRUD.h"
#include "parser.h"
#include "repository.h"
using namespace std::filesystem;

#pragma region StagingIndex Implementationi

void StagingIndex::InitializeClass(const path &indexFilePath)
{
    StagingIndex::indexFilePath = indexFilePath;
    load();
}

void StagingIndex::addEntry(const indexEntry &entry)
{
    StagingIndex::indexEntries.push_back(entry);
}
void StagingIndex::updateEntry(const path &filePath, const string &newHash, const string &newMode)
{
    indexEntry *entry = getEntry(filePath);
    if (!entry)
        return;

    entry->hash = newHash;
    entry->mode = newMode;
}
void StagingIndex::load()
{
    StagingIndex::indexEntries = {};
    string indexFileContents = readFile(StagingIndex::indexFilePath);
    vector<string> lines = split(indexFileContents, '\n');
    for (auto l : lines)
    {
        if (l.empty())
            continue;
        vector<string> parts = split(l, ' ');
        if (parts.size() == 4)
        {
            indexEntry iE(parts[0], parts[1], parts[2], parts[3]);
            StagingIndex::indexEntries.push_back(iE);
        }
    }
}
void StagingIndex::save()
{
    string indexFileContents = "";
    for (const auto &iE : indexEntries)
    {
        indexFileContents += iE.mode + " " + iE.hash + " " + iE.offset + " " + iE.path + "\n";
    }
    writeFile(indexFilePath, indexFileContents);
}

bool StagingIndex::isTrackedFile(const path &filePath)
{
    for (const auto &iE : indexEntries)
    {
        if (iE.path == filePath.generic_string())
        {
            return true;
        }
    }
    return false;
}

indexEntry *StagingIndex::getEntry(const path &filePath)
{
    for (auto &iE : indexEntries)
    {
        if (iE.path == filePath.generic_string())
        {
            return &iE;
        }
    }
    return nullptr;
}
bool StagingIndex::addFileToIndex(const path &filePath)
{
    if (!exists(filePath) || !is_regular_file(filePath))
    {
        cout << "Invalid file" << endl;
        return false;
    }

    path relPath = relative(absolute(filePath), Repository::project_absolute);
    string fileContents = readFile(filePath);
    GitObject blobObject = GitObject(Blob, fileContents);
    string hash = blobObject.getHash();
    string serializeBlobObject = blobObject.serialize();

    Repository::storeObject(blobObject);

    if (isTrackedFile(relPath))
    {
        updateEntry(relPath, hash, "100644");
        save();
        return true;
    }
    indexEntry iE("100644", hash, "0", relPath.generic_string());
    addEntry(iE);
    save();
    return true;
}


#pragma endregion