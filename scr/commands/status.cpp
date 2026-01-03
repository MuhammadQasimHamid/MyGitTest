#include <iostream>
#include "commands/status.h"
#include "dataStructure/cmpMap.h"
#include "core/repository.h"
#include "core/StagingIndex.h"
#include "utils/colors.h"
#include "dataStructure/Ntree.h"
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
    bool stagedChangesExist = false, unstagedChangesExist = false;
    cout << "On Branch " << Repository::currentBranch() << endl;

    cout << "Changes to be committed:" << endl; // index - last commit
    cout << "  (use 'git restore --staged <file>...' to unstage)";
    string cBranch = Repository::currentBranch();
    string cBranchHash = Repository::getBranchHash(cBranch);
    map<path, treeEntry> flattenTree;
    cmpMap<path, indexEntry, treeEntry> cmpMapiEtE;
    if (cBranchHash == "")
    {
    }
    else // fill the flatten tree
    {
        string rawFileContentsCommit = readFileWithStoredObjectHash(cBranchHash);
        CommitObject CObj(rawFileContentsCommit);
        string rawFileContentsTree = readFileWithStoredObjectHash(CObj.treeHash);
        TreeObject TObj(rawFileContentsTree);
        flattenTree = Repository::FlattenTreeObject(TObj);
        // key value1 value2
        for (auto iE : StagingIndex::indexEntries)
            cmpMapiEtE.addVal1(iE.path, iE);
        for (auto keyValuePair : flattenTree)
            cmpMapiEtE.addVal2(keyValuePair.first, keyValuePair.second);
    }

    setColor(0, 5);
    // cout << "Comparing with Last Commit: " << StagingIndex::indexEntries.size() <<"" << endl;
    for (auto cmpRow : cmpMapiEtE.umap)
    {
        const path &key = cmpRow.first;
        string filename = key.filename().string();
        cmpPair iEtEPair = cmpRow.second;
        cout << "";

        Cmp_Status cmpStatus = Repository::IndexCommitComp(iEtEPair.getVal1(), iEtEPair.getVal2().hash);
        if (cmpStatus == CMP_DIFFER)
        {
            stagedChangesExist = true;
            cout << RED << "modified:" << "    " << filename << endl;
        }
        else if (!iEtEPair.val2Exists()) // not exist in haeda
        {
            stagedChangesExist = true;
            cout << GREEN << "new file:" << "    " << filename << endl;
        }
        else if (!iEtEPair.val1Exists()) // not exist in index
        {
            stagedChangesExist = true;
            cout << RED << "deleted:" << "    " << filename << endl;
        }
    }
    cout << "End" << endl;
    setColor(0, 7);

    cout << "Changes not staged for commit:" << endl; // working dir - index

    for (auto iE : StagingIndex::indexEntries)
    {
        cout << "";
        Cmp_Status cmpStatus = Repository::IndexWorkingDirComp(iE, iE.path);
        // cout << fileStatusToS(fStatus) << " " << iE.path << endl;
        if (cmpStatus == CMP_DIFFER)
        {
            unstagedChangesExist = true;
            setColor(0, 5);
            cout << "modified" << " " << iE.path << endl;
            setColor(0, 7);
        }
        else if (cmpStatus == CMP_IN_WR_NotExist_WR)
        {
            unstagedChangesExist = true;
            setColor(0, 5);
            cout << "deleted" << " " << iE.path << endl;
            setColor(0, 7);
        }
    }

    cout << "End" << endl;
    if (!stagedChangesExist && !unstagedChangesExist)
    {
        cout << "Working Area Clean, nothing to commit" << endl;
    }
    traverseDirectory(".", [](const fs::path &relPath)
                      {
            if(!StagingIndex::isTrackedFile(relPath))
                std::cout << "Untracked: " << relPath.generic_string() << std::endl; });
}

// fs::path relPath;
// std::string rel;
//     for (fs::recursive_directory_iterator it(
//          Repository::project_absolute,
//          fs::directory_options::skip_permission_denied);
//      it != fs::recursive_directory_iterator();
//      ++it)
// {
//         const fs::directory_entry &entry = *it;

//         // skip hidden dirs
//         bool skip = false;
//         for (fs::path p = entry.path(); !p.empty(); p = p.parent_path())
//         {
//             auto name = p.filename().string();
//             if (name == ".git" || name == ".pit")
//             {
//                 if (entry.is_directory())
//                     it.disable_recursion_pending();
//                 skip = true;
//                 break;
//             }
//         }
//         if (skip)
//             continue; // safe early exit

//         // initialize after skip check
//         relPath = fs::relative(entry.path(), Repository::project_absolute);
//         rel = relPath.generic_string();

//         // check untracked
//         if (!StagingIndex::isTrackedFile(rel) && true)
//             // !Repository::isInPitIgnore(rel))
//         {
//             std::cout << RED << "Untracked: " << rel << std::endl;
//         }
//     }