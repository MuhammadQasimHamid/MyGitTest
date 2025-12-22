#include "commands/add.h"
#include <iostream>
#include "core/repository.h"
#include "core/StagingIndex.h"
using namespace std;

void addCommandExe(int argc, char *argv[])
{
    if (argc >= 3)
    {
        for (int i = 2; i < argc; i++)
        {
            if (!StagingIndex::addPathToIndex(argv[i]))
            {
                cout << argv[i] << " Folder Or file doen't exist!" << endl;
            }
        }
    }
    else
    {
        cout << "Invalid Params" << endl;
    }
}