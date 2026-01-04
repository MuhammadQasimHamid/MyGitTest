#ifndef BRANCH_H
#define BRANCH_H
#include <iostream>
#include <vector>
#include <string>

void branchCommandExe(int argc, char *argv[]);

void branchJSON(const std::vector<std::string> &branches, const std::string currentBranch);
#endif