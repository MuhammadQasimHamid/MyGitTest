#include "commands/log.h"
#include <iostream>
#include "core/repository.h"
#include "core/StagingIndex.h"
#include "utils/colors.h"
#include "utils/fileCRUD.h"
#include "core/gitObject.h"
#include <queue>
#include <set>
using namespace std;

void logCommandExe(int argc, char *argv[])
{
    if (argc == 2)
    {

        pair<string, string> headInfo = Repository::getHEAD();
        string tempHash;
        vector<string> allBranches = Repository::getAllBranches();
        string cBranch = Repository::currentBranch();
        if (headInfo.first == "hash:")
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
    else if (argc == 3 && string(argv[2]) == "--all")
    {
        queue<string> commitQueue;
        set<string> visitedCommits;
        vector<string> allBranches = Repository::getAllBranches();
        string cBranch = Repository::currentBranch();
        pair<string, string> headInfo = Repository::getHEAD();
        bool firstPrint = true;
        for (auto br : allBranches)
        {
            string branchHash = Repository::getBranchHash(br);
            if (branchHash != "" && visitedCommits.find(branchHash) == visitedCommits.end())
            {
                commitQueue.push(branchHash);
                visitedCommits.insert(branchHash);
            }
        }
        while (!commitQueue.empty())
        {
            string commitHash = commitQueue.front();
            commitQueue.pop();

            string rawCommitObjectContents = readFileWithStoredObjectHash(commitHash);
            CommitObject cObj(rawCommitObjectContents); // deserilize

            if (firstPrint)
            {
                cout << YELLOW << "Commit Hash: " << cObj.getHash() << "()" << RESETCOLOR << endl;
                firstPrint = false;
            }
            else
                cout << YELLOW << "Commit Hash: " << cObj.getHash() << RESETCOLOR << endl;
            
            cout << "Author: " << cObj.author << endl;
            cout << "Date: " << cObj.timeStamp << endl;
            cout << endl;
            cout << "     " << cObj.message;
            cout << endl;

            for (auto parentHash : cObj.parentHash)
            {
                if (visitedCommits.find(parentHash) == visitedCommits.end() && parentHash != "")
                {
                    commitQueue.push(parentHash);
                    visitedCommits.insert(parentHash);
                }
            }
        }
    }
    else
    {
        cout << "Invalid Params" << endl;
    }
}