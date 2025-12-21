#include "commands/commit.h"
#include <iostream>
#include "core/gitObject.h"
using namespace std;

void commitCommandExe(int argc, char *argv[])
{
    GitObject gitObj(Blob, "Umar Rana is here.");
    cout << "here is hash of my name" << endl;
    cout << gitObj.getHash() << endl;
}