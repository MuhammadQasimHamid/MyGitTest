#ifndef LOG_H
#define LOG_H
#include <iostream>
#include <string>
#include <vector>
#include <map>

struct LogCommit
{
    std::string hash;
    std::string message;
    std::string author;
    std::string timestamp;
    std::vector<std::string> parents;
};

void logCommandExe(int argc, char *argv[]);
void printLogJSON(
    const std::vector<LogCommit> &allCommits,
    const std::map<std::string, std::string> &branchWithCommitMap,
    const std::string &headHash);
void printingAuthorCommitter(std::string author, std::string timeStamp);
void getEmailAndNameFromAuthorString(const std::string &authorStr, std::string &name, std::string &email);
#endif