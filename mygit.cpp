#include <iostream>
#include <map>
#include "help.cpp"
#include "init.cpp"
#include "add.cpp"
#include "commit.cpp"
using namespace std;

typedef void (*cmdFunc)(int argc, char *argv[]);

// command functions
void initCommandExe(int argc, char *argv[]);
void addCommandExe(int argc, char *argv[]);
void helpCommandExe(int argc, char *argv[]);
void commitCommandExe(int argc, char *argv[]);

map<string, cmdFunc> cmdCodes;

void callFunc(int argc, char *argv[]);
void loadConfiguraion()
{
    cmdCodes["init"] = initCommandExe;
    cmdCodes["add"] = addCommandExe;
    cmdCodes["--help"] = helpCommandExe;
    cmdCodes["commit"] = commitCommandExe;
}
main(int argc, char *argv[])
{
    loadConfiguraion();
    cout << "Pit Refreshed" << endl;
    if (argc > 1)
    {
        callFunc(argc, argv);
    }
    return 0;
}

void callFunc(int argc, char *argv[])
{
    string cmdStr = argv[1];
    if (cmdCodes.find(cmdStr) == cmdCodes.end())
    {
        cout << "Invalid Command" << endl;
        return;
    }
    cmdCodes[cmdStr](argc, argv);
}