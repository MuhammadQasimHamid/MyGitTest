#ifndef ADD_H
#define ADD_H
#include<iostream>
#include "repository.cpp"
using namespace std;

void addCommandExe(int argc, char* argv[])
{
    if(argc == 3)
    {
        Repository R;
        if(!R.addFileToIndex(argv[2]))
        {
            cout << "Alreaduy" <<endl;
        }
    }
    else 
    {
        cout << "Invalid Params" << endl;
    }
}

#endif