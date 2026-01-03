#include "commands/catfile.h"
#include "core/repository.h"
#include <iostream>
using namespace std;

void catfileCommandExe(int argc, char *argv[])
{
    // pit cat-file -p  hash
    if (argc == 4)
    {
        string flag = argv[3 - 1];
        if (flag == "-p")
        {
            string hash = argv[3];
            if(storedObjectExistsWithHash(hash))
            {
                string fileContents = readFileWithStoredObjectHash(hash);
                GitObject Gobj(fileContents ); //calling deserialze constructor
                cout << "Raw File Contents: " << endl;
                cout << "---------------------------------------------" << endl;
                cout << fileContents;
                cout << "---------------------------------------------" << endl;
                cout << "Object Type: "<< GitObjectTypetos(Gobj.objectType) <<" Object contents:" << endl;
                cout << "---------------------------------------------" << endl;
                cout << Gobj.contents << endl;
                cout << "---------------------------------------------" << endl;
            }
            else 
            {
                cout << "Error file accessing the obhect" << endl;
            }
        }
    }
    else
    {
        cout << "Invalid Params for cat-file command." << endl;
    }
}
