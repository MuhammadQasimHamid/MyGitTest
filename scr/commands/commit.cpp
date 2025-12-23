#include "commands/commit.h"
#include <iostream>
#include "core/gitObject.h"
#include "core/repository.h"
using namespace std;

void commitCommandExe(int argc, char *argv[])
{
    // pit commit -m "OkOk"
    if (argc == 4)
    {
        string flag = argv[3 - 1];
        if (flag == "-m")
        {
            cout << "Commited " << endl;
            string msg = argv[4 - 1];
            Repository::generateCommit(msg);
        }
    }
}