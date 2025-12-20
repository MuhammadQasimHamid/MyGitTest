
#ifndef COMMIT_H
#define COMMIT_H
#include <iostream>
#include "gitObject.cpp"
using namespace std;

void commitCommandExe(int argc, char *argv[])
{
    GitObject gitObj(Blob, "Umar Rana is here.");
    cout << "here is hash of my name" << endl;
    cout << gitObj.getHash() << endl;
}

#endif