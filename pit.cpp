#include <iostream>
#include <map>
#include "commands/help.h"
#include "commands/init.h"
#include "commands/add.h"
#include "commands/commit.h"
#include "commands/status.h"
#include "commands/branch.h"
#include "commands/merge.h"
#include "commands/config.h"
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
void catfileCommandExe(int argc, char *argv[]);
void logCommandExe(int argc, char *argv[]);
void configCommandExe(int argc, char *argv[]);
map<string, cmdFunc> cmdCodes;

void callFunc(int argc, char *argv[]);
bool pitRepoExists();
void printArgs(int argc, char *argv[]);
void loadConfiguraion()
{
    cmdCodes["init"] = initCommandExe;
    cmdCodes["add"] = addCommandExe;
    cmdCodes["--help"] = helpCommandExe;
    cmdCodes["commit"] = commitCommandExe;
    cmdCodes["status"] = statusCommandExe;
    cmdCodes["checkout"] = checkoutCommandExe;
    cmdCodes["cat-file"] = catfileCommandExe;
    cmdCodes["log"] = logCommandExe;
    cmdCodes["branch"] = branchCommandExe;
    cmdCodes["merge"] = mergeCommandExe;
    cmdCodes["config"] = configCommandExe;

    // if (pitRepoExists())
        Repository::InitializeClass();
}
int main(int argc, char *argv[])
{
    // printArgs(argc, argv);
    loadConfiguraion();
    // cout << "Pit Refreshed" << endl;
    if (argc > 1)
    {
        callFunc(argc, argv);
    }
    else if (true)
    {
        // Repository::InitializeClass();
        cout << "Function Testing---Start" << endl;
        // string fileContents = readFileWithStoredObjectHash("4c3lla28fee213ff64818e0ec871e401639071fda2"); done
        // cout << fileContents << endl;    done
        // initCommandExe(2, (char *[]){"", "init"});
        // addCommandExe(3, (char*[]){"", "add", "."});
        // char* ar[]= {"","commit","-m","test"};
        // commitCommandExe(4,ar);
        // statusCommandExe(argc, argv);
        // branchCommandExe(2,(char*[]){"","branch"});
        // logCommandExe(3,(char*[]){"","log","--all"});
        // checkoutCommandExe(3, (char*[]){"","checkout","456446c04ab6636209925c44d7573e54f02c1aec"});
        // cout << Repository::getBranchHash("master")  ; done
        // configCommandExe(4, (char *[]){"pit" ,"config", "user.email" ,"Qasim@"});
        callFunc(3,(char*[]){"pit","checkout","dev"});
        cout << "Function Testing---End" << endl;
        system("pause");
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
    if (pitRepoExists() || cmdStr == "init")
    {
        // Repository::InitializeClass();
        cmdCodes[cmdStr](argc, argv);
    }
    else
    {
        cout << "Pit repo not exists in this directory..." << endl;
        cout << "(Type: 'pit init' to initialize pit repo)." << endl;
    }
}

bool pitRepoExists()
{
    if (exists(".pit"))
    {
        return true;
    }
    return false;
}

void printArgs(int argc, char *argv[])
{
    cout << "Arg Count: " << argc << endl;
    cout << "Arg Values: ";
    for (int i = 0; i < argc; i++)
    {
        cout << argv[i] << " ";
    }
    cout << endl;
}