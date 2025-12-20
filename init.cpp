#include <iostream>
#include "repository.cpp"
using namespace std;

void initCommandExe(int argc, char *argv[])
{
    Repository repo;
    if (!repo.initRepo())
        cout << "Repository Already exist.";
}