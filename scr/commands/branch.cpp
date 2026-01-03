#include "commands/branch.h"
#include <iostream>
#include "core/repository.h"
#include "core/StagingIndex.h"
using namespace std;

void branchCommandExe(int argc, char *argv[])
{
    if (argc == 3)
    {
        string newBranchName = argv[2];
        // path newBranchPath = path(".pit/refs/heads/" + newBranchName);
        fs::path newbranchPath = Repository::refsHeadFolderPath/newBranchName;
        cout << newbranchPath << endl;
        if (!exists(newbranchPath))
        {
            string cHash = Repository::BranchPointToHashOrNothing(Repository::currentBranch());
            if (cHash.empty())
            {
                cout << "fatal: not valid branch" << endl;
                return;
            }
            writeFile(newbranchPath, cHash);
        }
        else
        {
            cout << "fatal: a branch named '" << newBranchName << "' already exists" << endl;
        }
    }
    else
    {
        cout << "Invalid Params" << endl;
    }
}