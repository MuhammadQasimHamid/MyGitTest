#include "add.h"
#include<iostream>
#include "repository.h"
#include "StagingIndex.h"
using namespace std;

void addCommandExe(int argc, char* argv[])
{
    if(argc == 3)
    {
        if(!StagingIndex::addFileToIndex(argv[2]))
        {
            cout << "Alreaduy" <<endl;
        }
    }
    else 
    {
        cout << "Invalid Params" << endl;
    }
}