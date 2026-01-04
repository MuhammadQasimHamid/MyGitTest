#include "commands/branch.h"
#include <iostream>
#include "core/repository.h"
#include "core/StagingIndex.h"
#include "utils/colors.h"
#include "utils/json_helper.h"
using namespace std;

void branchCommandExe(int argc, char *argv[])
{
    bool jsonMode = false;
    for (int i = 1; i < argc; ++i)
        if (string(argv[i]) == "--json")
            jsonMode = true;
    if (jsonMode)
    {
        vector<string> allBranches = Repository::getAllBranches();
        string cBranch = Repository::currentBranch();
        branchJSON(allBranches, cBranch);
        return;
    }
    if (argc == 2)
    {
        vector<string> allBranches = Repository::getAllBranches();
        string cBranch = Repository::currentBranch();
        for (auto br : allBranches)
        {
            if (br == cBranch)
                cout << "* " << GREEN << br << RESETCOLOR << endl;
            else
                cout << "  " << br << endl;
        }
    }
    else if (argc == 3)
    {
        string newBranchName = argv[2];
        // path newBranchPath = path(".pit/refs/heads/" + newBranchName);
        fs::path newbranchPath = Repository::refsHeadFolderPath / newBranchName;
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

void branchJSON(
    const std::vector<std::string> &branches, const std::string currentBranch)
{
    using namespace json;

    beginObject();
    printArray("branches", branches);
    printString("current_branch", currentBranch, false);
    endObject();
}