#include "commands/log.h"
#include <iostream>
#include "core/repository.h"
#include "core/StagingIndex.h"
#include "utils/colors.h"
#include "utils/fileCRUD.h"
#include "core/gitObject.h"
#include "utils/json_helper.h"
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
    else if (argc == 4 && string(argv[2]) == "--all" && string(argv[3]) == "--json")
    {
        queue<string> commitQueue;
        set<string> visitedCommits;
        vector<string> allBranches = Repository::getAllBranches();
        string cBranch = Repository::currentBranch();
        pair<string, string> headInfo = Repository::getHEAD();

        // making data to form json
        vector<LogCommit> allCommits;
        map<std::string, std::string> branchWithCommitMap;
        string currentBranchHash = Repository::getBranchHash(cBranch);
        for (auto br : allBranches)
        {
            string branchHash = Repository::getBranchHash(br);
            branchWithCommitMap[br] = branchHash;
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

            allCommits.push_back(
                {commitHash,
                 cObj.message,
                 cObj.author,
                 cObj.timeStamp,
                 cObj.parentHash});
            for (auto parentHash : cObj.parentHash)
            {
                if (visitedCommits.find(parentHash) == visitedCommits.end() && parentHash != "")
                {
                    commitQueue.push(parentHash);
                    visitedCommits.insert(parentHash);
                }
            }
        }
        printLogJSON(allCommits, branchWithCommitMap, currentBranchHash);
    }
    else
    {
        cout << "Invalid Params" << endl;
    }
}

void printLogJSON(
    const std::vector<LogCommit> &allCommits,
    const std::map<std::string, std::string> &branchWithCommitMap,
    const std::string &headHash)
{
    using namespace json;
    beginObject();

    // commits
    cout << "  \"commits\": [\n";
    for (size_t i = 0; i < allCommits.size(); i++)
    {
        const LogCommit &c = allCommits[i];

        cout << "    {\n";
        printString("hash", c.hash, true);
        printString("message", c.message, true);
        printString("author", c.author, true);
        printString("timestamp", c.timestamp, true);
        printArray("parents", c.parents, false);
        cout << "    }";

        if (i + 1 < allCommits.size())
            cout << ",";
        cout << "\n";
    }
    cout << "  ],\n";

    // head
    printString("head", headHash, true);

    // branches
    beginNamedObject("branches");
    size_t count = 0;
    for (auto &it : branchWithCommitMap)
    {
        cout << "    \"" << escape(it.first) << "\": "
             << "\"" << escape(it.second) << "\"";

        if (++count < branchWithCommitMap.size())
            cout << ",";
        cout << "\n";
    }
    endNamedObject(false);

    endObject(false);
}