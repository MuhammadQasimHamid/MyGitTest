#include <iostream>
#include <vector>
#include <filesystem>
#include "fileCRUD.cpp"
#include "parser.cpp"
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
    static void InitializeClass(const path &indexFilePath)
    {
        StagingIndex::indexFilePath = indexFilePath;
        load();
    }

    static void addEntry(const indexEntry &entry)
    {
        StagingIndex::indexEntries.push_back(entry);
    }
    static void updateEntry(const path &filePath, const string &newHash, const string &newMode)
    {
        indexEntry *entry = getEntry(filePath);
        if (!entry)
            return;

        entry->hash = newHash;
        entry->mode = newMode;
    }
    static void load()
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
    static void save()
    {
        string indexFileContents = "";
        for (const auto &iE : indexEntries)
        {
            indexFileContents += iE.mode + " " + iE.hash + " " + iE.offset + " " + iE.path + "\n";
        }
        writeFile(indexFilePath, indexFileContents);
    }

    static bool isTrackedFile(const path &filePath)
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

    static indexEntry *getEntry(const path &filePath)
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
};