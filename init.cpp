#ifndef INIT_H
#define INIT_H

#include <iostream>
#include "repository.cpp"
using namespace std;

void initCommandExe(int argc, char *argv[])
{
    Repository repo;
    if (!repo.initRepo())
        cout << "Repository Already exist.";
    repo.storeObject(GitObject(Blob,"This is a test object"));
}


#endif