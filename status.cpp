#ifndef STATUS_H
#define STATUS_H

#include <iostream>
#include "repository.cpp"
using namespace std;

void statusCommandExe(int argc, char *argv[])
{
    Repository R;
    cout << "On Branch " << R.currentBranch();
}


#endif