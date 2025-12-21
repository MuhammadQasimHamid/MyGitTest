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
    vector<indexEntry> indexEntries;
    path indexFilePath;
    StagingIndex(const path &indexFilePath)
    {
        this->indexFilePath = indexFilePath;
        load();
    }

    void addEntry(const indexEntry &entry)
    {
        this->indexEntries.push_back(entry);
    }
    void updateEntry(const path &filePath, const string &newHash, const string &newMode)
    {
        indexEntry *entry = getEntry(filePath);
        if (!entry)
            return;

        entry->hash = newHash;
        entry->mode = newMode;
    }
    void load()
    {
        this->indexEntries = {};
        string indexFileContents = readFile(this->indexFilePath);
        vector<string> lines = split(indexFileContents, '\n');
        for (auto l : lines)
        {
            if (l.empty())
                continue;
            vector<string> parts = split(l, ' ');
            if (parts.size() == 4)
            {
                indexEntry iE(parts[0], parts[1], parts[2], parts[3]);
                this->indexEntries.push_back(iE);
            }
        }
    }
    void save()
    {
        string indexFileContents = "";
        for (const auto &iE : this->indexEntries)
        {
            indexFileContents += iE.mode + " " + iE.hash + " " + iE.offset + " " + iE.path + "\n";
        }
        writeFile(this->indexFilePath, indexFileContents);
    }

    bool isTrackedFile(const path &filePath)
    {
        for (const auto &iE : this->indexEntries)
        {
            if (iE.path == filePath.generic_string())
            {
                return true;
            }
        }
        return false;
    }

    indexEntry *getEntry(const path &filePath)
    {
        for (auto &iE : this->indexEntries)
        {
            if (iE.path == filePath.generic_string())
            {
                return &iE;
            }
        }
        return nullptr;
    }
};