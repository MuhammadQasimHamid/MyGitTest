#include "init.h"
#include <iostream>
#include "repository.h"
using namespace std;

void initCommandExe(int argc, char *argv[])
{
    
    if (!Repository::initRepo())
        cout << "Repository Already exist.";
}
