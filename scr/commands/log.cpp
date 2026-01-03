#include "commands/log.h"
#include <iostream>
#include "core/repository.h"
#include "core/StagingIndex.h"
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
            TreeObject tObj(readFileWithStoredObjectHash(cObj.treeHash));
            cout << "------------------" << cObj.message << " " << " Hash: " << cObj.getHash() << "----------------" << endl;
            cout << cObj.serializeContent() << endl;
            cout <<  "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << endl;
            cout << tObj.contents << endl;
            tempHash = cObj.parentHash.empty()? "" : cObj.parentHash[0];
        }
    }
    else
    {
        cout << "Invalid Params" << endl;
    }
}