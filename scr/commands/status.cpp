#include "commands/status.h"
#include <iostream>
#include "core/repository.h"
using namespace std;

void statusCommandExe(int argc, char *argv[])
{
    cout << "On Branch " << Repository::currentBranch();
}
