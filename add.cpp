#include<iostream>
using namespace std;

void addCommandExe(int argc, char* argv[])
{
    if(argc == 3)
    {
        string filename = argv[2];
        cout << filename << " created now" << endl; 
    }
    else 
    {
        cout << "Invalid Params" << endl;
    }
}