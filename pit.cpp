#include <iostream>
#include <map>
#include "commands/help.h"
#include "commands/init.h"
#include "commands/add.h"
#include "commands/commit.h"
#include "commands/status.h"
#include "core/repository.h"
#include "core/StagingIndex.h"

using namespace std;

typedef void (*cmdFunc)(int argc, char *argv[]);

// command functions
void initCommandExe(int argc, char *argv[]);
void addCommandExe(int argc, char *argv[]);
void helpCommandExe(int argc, char *argv[]);
void commitCommandExe(int argc, char *argv[]);
void statusCommandExe(int argc, char *argv[]); 
void checkoutCommandExe(int argc, char *argv[]); 
map<string, cmdFunc> cmdCodes;

void callFunc(int argc, char *argv[]);
bool pitRepoExists(); 
void loadConfiguraion()
{
    cmdCodes["init"] = initCommandExe;
    cmdCodes["add"] = addCommandExe;
    cmdCodes["--help"] = helpCommandExe;
    cmdCodes["commit"] = commitCommandExe;
    cmdCodes["status"] = statusCommandExe;
    cmdCodes["checkout"] = checkoutCommandExe;
    Repository::InitializeClass();
}
int main(int argc, char *argv[])
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
    if(pitRepoExists() || cmdStr == "init")
    {
        cmdCodes[cmdStr](argc, argv);
    }
    else
    {   
        cout << "Pit repo not exists in this directory..." <<endl;
        cout << "(Type: 'pit init' to initialize pit repo)." << endl;
    }
}

bool pitRepoExists()
{
    if(exists(".pit"))
    {
        return true;
    }
    return false;
}