#include <iostream>
#include "sha1.cpp"
using namespace std;

void initCommandExe(int argc, char *argv[])
{
    cout << sha1("example") << endl;
    cout << "Repository initialized." << endl;
    cout << "Init command executed." << endl;
    cout << sha1("umarranag") << endl;
    cout << sha1("qasim") << endl;
    cout << sha1("hamid") << endl;
}