#ifndef STATUS_H
#define STATUS_H
#include <string>
#include <iostream>
#include <vector>
using namespace std;
void statusCommandExe(int argc, char *argv[]);
void statusJSON(
    const std::string &branch,
    const std::vector<std::string> &staged,
    const std::vector<std::string> &unstaged,
    const std::vector<std::string> &untracked);

#endif