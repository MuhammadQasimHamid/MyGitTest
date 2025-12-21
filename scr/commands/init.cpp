#include "commands/init.h"
#include <iostream>
#include "core/repository.h"
using namespace std;

void initCommandExe(int argc, char *argv[])
{
    cout << "Proved2" << endl;
    if (!Repository::initRepo())
        cout << "Repository Already exist.";
}
