#include <iostream>
#include "sha1.cpp"
using namespace std;

void initCommandExe(int argc, char *argv[])
{
    cout << sha1("example");
}