#include "commands/log.h"
#include <iostream>
#include "core/repository.h"
#include "core/StagingIndex.h"
#include "utils/colors.h"
using namespace std;

void logCommandExe(int argc, char *argv[])
{
    if (argc == 2)
    {
        string cBranch = Repository::currentBranch();
        string cBranchHash = Repository::BranchPointToHashOrNothing(cBranch);
        string tempHash = cBranchHash;
        while (tempHash != "")
        {
            string rawCommitObjectContents = readFileWithStoredObjectHash(tempHash);
            CommitObject cObj(rawCommitObjectContents); // deserilize
            if (cObj.parentHash.empty())
                cout << YELLOW << "Commit Hash: " << cObj.getHash() << RESETCOLOR  << () << endl;
            else
                cout << YELLOW << "Commit Hash: " << cObj.getHash() << RESETCOLOR << endl;
            cout << "Author: " << cObj.author << endl;
            cout << "Date: " << cObj.timeStamp << endl;
            cout << endl;
            cout << "     " << cObj.message;
            cout << endl;
            tempHash = cObj.parentHash.empty() ? "" : cObj.parentHash[0];
            // TreeObject tObj(readFileWithStoredObjectHash(cObj.treeHash));
            // cout << "------------------" << cObj.message << " " << " Hash: " << cObj.getHash() << "----------------" << endl;
            // cout << cObj.serializeContent() << endl;
            // cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << endl;
            // cout << tObj.contents << endl;
        }
    }
    else
    {
        cout << "Invalid Params" << endl;
    }
}