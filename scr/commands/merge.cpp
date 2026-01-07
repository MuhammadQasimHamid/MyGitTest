#include "commands/merge.h"
#include <iostream>
#include "core/repository.h"
#include "core/StagingIndex.h"
using namespace std;

void mergeCommandExe(int argc, char *argv[])
{
    if (argc >= 3)
    {
        string branchToMerge = argv[3 - 1];
        if (Repository::isaBranch(branchToMerge))
        {
            cout << "Merging branch: " << branchToMerge << endl;
            Repository::mergeBranch(branchToMerge);
        }
    }
    else
    {
        cout << "Invalid Params" << endl;
    }
}