#include "core/StagingIndex.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include "utils/fileCRUD.h"
#include "utils/myparser.h"
#include "core/repository.h"
using namespace std::filesystem;

#pragma region StagingIndex Implementationi

vector<indexEntry> StagingIndex::indexEntries;
path StagingIndex::indexFilePath;

void StagingIndex::InitializeClass(const path &indexFilePath)
{
    StagingIndex::indexFilePath = indexFilePath;
    load();
}

 bool StagingIndex::IndexHasConflicts()
{
    for(auto iE:indexEntries)
    {
        if(iE.offset!="0")
            return true;
    }
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
    try
    {
        if (exists(indexFilePath))
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
    }
    catch (...)
    {
        // just ignore
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
        path relPath = relative(absolute(filePath), Repository::project_absolute);
        if (iE.path == relPath) // compare paths correctly
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
        if (iE.path == relative(absolute(filePath), Repository::project_absolute).string()) // compare paths correctly
        {
            return &iE;
        }
    }
    return nullptr;
}
bool StagingIndex::addPathToIndex(const path &dirPath)
{
    path relativePath = relative(absolute(dirPath), Repository::project_absolute);
    if (!exists(relativePath))
        return false;
    if (is_directory(relativePath))
    {
        addDirectory(relativePath);
        if (relativePath == ".")
        {
            removeDeletedWRFilesFromIndex();
            save(); // update
        }
    }
    else if (is_regular_file(relativePath))
    {
        addFileToIndex(relativePath);
    }
    else
    {
        return false;
    }
    return true;
}
int StagingIndex::removeDeletedWRFilesFromIndex()
{
    int count = 0;
    for (auto it = indexEntries.begin(); it != indexEntries.end();)
    {
        if (Repository::IndexWorkingDirComp(*it, it->path) == CMP_IN_WR_NotExist_WR)
        {
            it = indexEntries.erase(it);
            count++;
        }
        else
            ++it;
    }
    return count;
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
    BlobObject blobObj = BlobObject(filePath.filename().string(), fileContents);
    string hash = blobObj.getHash();
    string serializeBlobObject = blobObj.serialize();

    Repository::storeObject(blobObj);

    if (isTrackedFile(relPath))
    {
        updateEntry(relPath, hash, "100644");
        save();
        return true;
    }
    indexEntry iE("100644", hash, "0", relPath.string());
    addEntry(iE);
    save();
    cout << filePath << " added" << endl;
    return true;
}

bool StagingIndex::addDirectory(const path &dirPath)
{
    if (!exists(dirPath) || !is_directory(dirPath))
    {
        return false;
    }
    try
    {
        // Use standard iterator to gain access to disable_recursion_pending()
        for (auto it = recursive_directory_iterator(dirPath); it != recursive_directory_iterator(); ++it)
        {
            const auto &entry = *it;
            string filename = entry.path().filename().string();

            // Check if current directory is .git or .pit
            if (is_directory(entry.path()) && (filename == ".git" || filename == ".pit" || filename == "node_modules" || filename == ".svn" || filename == ".vscode" || filename == "__pycache__" || filename == ".gitignore" || filename == "build"))
            {
                cout << "Skipping system directory and its contents: " << entry.path() << endl;
                // This prevents the iterator from entering this folder
                it.disable_recursion_pending();
                continue;
            }
            if (is_regular_file(entry.path()))
            {
                addFileToIndex(entry.path());
            }
        }
    }
    catch (const filesystem_error &e)
    {
        cerr << "Filesystem error: " << e.what() << endl;
        return false;
    }
    return true;
}

#pragma endregion