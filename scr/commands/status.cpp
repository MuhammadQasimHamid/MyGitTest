#include <iostream>
#include "commands/status.h"
#include "dataStructure/cmpMap.h"
#include "core/repository.h"
#include "core/StagingIndex.h"
#include "utils/colors.h"
#include "dataStructure/Ntree.h"
#include "utils/json_helper.h"

using namespace std;
// #undef byte
// #include <windows.h>
void setColor(int fore, int back)
{
    // HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // SetConsoleTextAttribute(hConsole, (fore * 16) + back); //
}
void statusCommandExe(int argc, char *argv[])
{
    // parse args early so we can suppress human output when --json is given
    bool jsonMode = false;
    for (int i = 1; i < argc; ++i)
        if (string(argv[i]) == "--json")
            jsonMode = true;

    struct FileChange
    {
        string path;
        string type;
    };
    vector<FileChange> stagedChanges;   // keeps path + type (modified/new/deleted)
    vector<FileChange> unstagedChanges; // keeps path + type (modified/deleted)
    vector<string> stagedFiles;         // for json output (paths)
    vector<string> unstagedFiles;       // for json output (paths)
    vector<string> untrackedFiles;      // for json output (paths)

    const string branch = Repository::currentBranch();
    const string cBranchHash = Repository::getBranchHash(branch);

    // Build compare map between index and last commit
    map<path, treeEntry> flattenTree;
    cmpMap<path, indexEntry, treeEntry> cmpMapiEtE;
    for (const auto &iE : StagingIndex::indexEntries)
        if (!StagingIndex::isaConflictFile(iE.path))
            cmpMapiEtE.addVal1(iE.path, iE);

    if (!cBranchHash.empty())
    {
        string rawFileContentsCommit = readFileWithStoredObjectHash(cBranchHash);
        CommitObject CObj(rawFileContentsCommit);
        string rawFileContentsTree = readFileWithStoredObjectHash(CObj.treeHash);
        TreeObject TObj(rawFileContentsTree);
        flattenTree = Repository::FlattenTreeObject(TObj);
        for (auto &kv : flattenTree)
        {
            if (!StagingIndex::isaConflictFile(kv.first))
                cmpMapiEtE.addVal2(kv.first, kv.second);
        }
    }

    // Compare index vs last commit (staged changes)
    for (auto &cmpRow : cmpMapiEtE.umap)
    {
        const path &key = cmpRow.first;
        const string filepath = key.generic_string();
        cmpPair iEtEPair = cmpRow.second;
        Cmp_Status cmpStatus = Repository::IndexCommitComp(iEtEPair.getVal1(), iEtEPair.getVal2().hash);
        if (cmpStatus == CMP_DIFFER)
        {
            stagedChanges.push_back({filepath, "modified"});
            stagedFiles.push_back(filepath);
        }
        else if (!iEtEPair.val2Exists()) // not exist in head
        {
            stagedChanges.push_back({filepath, "new file"});
            stagedFiles.push_back(filepath);
        }
        else if (!iEtEPair.val1Exists()) // not exist in index
        {
            stagedChanges.push_back({filepath, "deleted"});
            stagedFiles.push_back(filepath);
        }
    }

    // Compare working dir vs index (unstaged changes)
    for (const auto &iE : StagingIndex::indexEntries)
    {
        Cmp_Status cmpStatus = Repository::IndexWorkingDirComp(iE, iE.path);
        if (cmpStatus == CMP_DIFFER)
        {
            unstagedChanges.push_back({iE.path, "modified"});
            unstagedFiles.push_back(iE.path);
        }
        else if (cmpStatus == CMP_IN_WR_NotExist_WR)
        {
            unstagedChanges.push_back({iE.path, "deleted"});
            unstagedFiles.push_back(iE.path);
        }
    }

    // Collect untracked files (always collect, print only in human mode)
    traverseDirectory(".", [&](const fs::path &relPath)
                      {
        if (!StagingIndex::isTrackedFile(relPath))
            untrackedFiles.push_back(relPath.generic_string()); });

    // If json mode, print only JSON and exit early
    if (jsonMode)
    {
        statusJSON(branch, stagedFiles, unstagedFiles, untrackedFiles);
        return;
    }

    // Human readable output
    cout << "On Branch " << branch << endl
         << endl;

    if (stagedChanges.size() > 0)
    {
        cout << "Changes to be committed:" << endl; // index - last commit
        cout << "  (use 'git restore --staged <file>...' to unstage)" << endl;
        if (stagedChanges.empty())
        {
            cout << "\t(no changes added to commit)" << endl;
        }
        else
        {
            for (const auto &c : stagedChanges)
            {
                const auto filename = fs::path(c.path).filename().string();
                if (c.type == "modified")
                    cout << GREEN << "\tmodified:" << "    " << filename << RESETCOLOR << endl;
                else if (c.type == "new file")
                    cout << GREEN << "\tnew file:" << "    " << filename << RESETCOLOR << endl;
                else
                    cout << GREEN << "\tdeleted:" << "    " << filename << RESETCOLOR << endl;
            }
        }
    }

    if (unstagedFiles.size() > 0)
    {
        cout << "Changes not staged for commit:" << endl;
        if (unstagedChanges.empty())
        {
            cout << "\t(none)" << endl;
        }
        else
        {
            for (const auto &c : unstagedChanges)
            {
                if (c.type == "modified")
                    cout << RED << "\tmodified" << " " << c.path << RESETCOLOR << endl;
                else
                    cout << RED << "\tdeleted" << " " << c.path << RESETCOLOR << endl;
            }
        }
    }
    if (stagedChanges.empty() && unstagedChanges.empty())
        cout << "Working Area Clean, nothing to commit" << endl;

    if (untrackedFiles.size() > 0)
    {
        cout << "Untracked files:" << endl;
        cout << "(use \" git add<file>... \" to include in what will be committed)" << endl;
        for (const auto &u : untrackedFiles)
            cout << RED << "\tUntracked: " << u << RESETCOLOR << endl;
    }

    if (StagingIndex::IndexHasConflicts())
    {
        cout << "Unmerged paths:" << endl;
        for (auto pair : flattenTree)
        {
            if (StagingIndex::isaConflictFile(pair.first))
            {
                cout << RED << "\tconflict: " << pair.first << RESETCOLOR << endl;
            }
        }
    }
}

void statusJSON(
    const std::string &branch,
    const std::vector<std::string> &staged,
    const std::vector<std::string> &unstaged,
    const std::vector<std::string> &untracked)
{
    using namespace json;

    beginObject();
    printString("branch", branch);
    printArray("staged", staged);
    printArray("unstaged", unstaged);
    printArray("untracked", untracked, false);
    endObject();
}