#include "commands/status.h"
#include <iostream>
#include <windows.h>
#include "core/repository.h"
#include "core/StagingIndex.h"
using namespace std;
void setColor(int fore, int back)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (fore * 16) + back); //
}
void statusCommandExe(int argc, char *argv[])
{
    bool stagedChangesExist = false, unstagedChangesExist = false;
    cout << "On Branch " << Repository::currentBranch() << endl;

    cout << "Changes to be committed:" << endl;
    string cBranch = Repository::currentBranch();
    string cBranchHash = Repository::getBranchHash(cBranch);
    string rawFileContentsCommit = readFileWithStoredObjectHash(cBranchHash);
    CommitObject CObj(rawFileContentsCommit);
    string rawFileContentsTree = readFileWithStoredObjectHash(CObj.treeHash);
    TreeObject TObj(rawFileContentsTree);
    map<path, treeEntry> flattenTree = Repository::FlattenTreeObject(TObj);
    // cout << "Comparing with Last Commit: " << StagingIndex::indexEntries.size() <<"" << endl;
    for (auto iE : StagingIndex::indexEntries)
    {
        cout << "                 ";
        if (flattenTree.find(iE.path) != flattenTree.end())
        {
            FileStatus fStatus = Repository::IndexLastCommitComp(iE, (flattenTree[iE.path]).hash);
            setColor(0, 5);
            if (fStatus == File_ContentsDiffer)
            {
                stagedChangesExist = true;
                cout << "modified" << " " << iE.path << endl;
            }
            setColor(0, 7);
        }
        else
        {
            cout << "not exist " << iE.path << endl;
        }
    }
    cout << "End" << endl;

    cout << "Changes not staged for commit:" << endl;
    for (auto iE : StagingIndex::indexEntries)
    {
        cout << "                 ";
        FileStatus fStatus = Repository::IndexWorkingDirComp(iE, iE.path);
        // cout << fileStatusToS(fStatus) << " " << iE.path << endl;
        if (fStatus == File_ContentsDiffer)
        {
            unstagedChangesExist = true;
            setColor(0, 5);
            cout << "modified" << " " << iE.path << endl;
            setColor(0, 7);
        }
    }
    cout << "End" << endl;
    if (!stagedChangesExist && !unstagedChangesExist)
    {
        cout << "Working Area Clean, nothing to commit" << endl;
    }

    cout << "Untracked Files" << endl;
    for (auto it : recursive_directory_iterator(Repository::project_absolute))
    {
        if (!StagingIndex::isTrackedFile(it.path()) && !Repository::isInPitIgnore(it.path()))
        {
            cout << "Untracked: " << it.path().string() << endl;
        }
    }
}
