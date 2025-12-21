#include "status.h"
#include <iostream>
#include "repository.h"
using namespace std;

void statusCommandExe(int argc, char *argv[])
{
    cout << "On Branch " << Repository::currentBranch();
}
