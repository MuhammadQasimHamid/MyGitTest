#ifndef INIT_H
#define INIT_H

#include <iostream>
#include "repository.cpp"
using namespace std;

void initCommandExe(int argc, char *argv[])
{
    
    if (!Repository::initRepo())
        cout << "Repository Already exist.";

    Repository::addFileToIndex("init.cpp");
    Repository::addFileToIndex("add.cpp");
}

#endif