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
    cout << "On Branch " << Repository::currentBranch();

    cout << "Changes staged for commit:" << endl;
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
        cout << "           ";
        if (flattenTree.find(iE.path) != flattenTree.end())
        {
            FileStatus fStatus = Repository::IndexLastCommitComp(iE, (flattenTree[iE.path]).hash);
            setColor(0,5);
            if(fStatus == File_ContentsDiffer)
                cout << "modified" << " " << iE.path << endl;
            setColor(0,7);
        }
        else
        {
            cout << "not exist " << iE.path << endl;
        }
    }
    cout << "End" << endl;

    cout << "Changes to be committed:" << endl;
    for (auto iE : StagingIndex::indexEntries)
    {
        cout << "                     ";
        FileStatus fStatus = Repository::IndexWorkingDirComp(iE, iE.path);
        if (fStatus == File_Same)
        {
            cout << " " << iE.path << endl;
        }
    }
    cout << "End" << endl;

    
    // cout << "Untracked Files" << endl;
    // for(auto it:recursive_directory_iterator(Repository::project_absolute))
    // {
    //     if(!StagingIndex::isTrackedFile(it.path()) && is_regular_file(it.path()))
    //     {
    //         cout << "Untracked: " << it.path().string() << endl;
    //     }
    // }
}
