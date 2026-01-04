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
        
        pair<string, string> headInfo = Repository::getHEAD();
        string tempHash;
        vector<string> allBranches = Repository::getAllBranches();
        string cBranch = Repository::currentBranch();
        if(headInfo.first == "hash:")
        {
            tempHash = headInfo.second;
        }
        else
        {
            tempHash = Repository::BranchPointToHashOrNothing(cBranch);
        }
        
        bool firstPrint = true;
        while (tempHash != "")
        {
            string rawCommitObjectContents = readFileWithStoredObjectHash(tempHash);
            CommitObject cObj(rawCommitObjectContents); // deserilize

            if (firstPrint)
            {
                if (headInfo.first == "hash")
                {
                    cout << YELLOW << "Commit Hash: " << cObj.getHash() << RESETCOLOR << "(HEAD)" << endl;
                }
                else
                {
                    string branches = "->" + cBranch;
                    for (auto br : allBranches)
                    {
                        branches += (br == cBranch) ? "" : "," + br;
                    }
                    cout << YELLOW << "Commit Hash: " << cObj.getHash() << RESETCOLOR << "(HEAD" << branches << ")" << endl;
                }
                firstPrint = false;
            }
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