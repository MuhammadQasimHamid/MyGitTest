#include <iostream>
#include <map>
#include "include/commands/help.h"
#include "include/commands/init.h"
#include "include/commands/add.h"
#include "include/commands/commit.h"
#include "include/commands/status.h"
#include "include/core/repository.h"
#include "include/core/StagingIndex.h"

using namespace std;

typedef void (*cmdFunc)(int argc, char *argv[]);

// command functions
void initCommandExe(int argc, char *argv[]);
void addCommandExe(int argc, char *argv[]);
void helpCommandExe(int argc, char *argv[]);
void commitCommandExe(int argc, char *argv[]);
void statusCommandExe(int argc, char *argv[]); 
map<string, cmdFunc> cmdCodes;

void callFunc(int argc, char *argv[]);
bool mygitRepoExists(); 
void loadConfiguraion()
{
    cmdCodes["init"] = initCommandExe;
    cmdCodes["add"] = addCommandExe;
    cmdCodes["--help"] = helpCommandExe;
    cmdCodes["commit"] = commitCommandExe;
    cmdCodes["status"] = statusCommandExe;
    Repository::InitializeClass();
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
    if(mygitRepoExists() || cmdStr == "init")
    {
        cmdCodes[cmdStr](argc, argv);
    }
    else
    {   
        cout << "Pit repo not exists in this directory..." <<endl;
        cout << "(Type: 'pit init' to initialize pit repo)." << endl;
    }
}

bool mygitRepoExists()
{
    if(exists(".pit"))
    {
        return true;
    }
    return false;
}