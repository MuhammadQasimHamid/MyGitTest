#include "commands/add.h"
#include <iostream>
#include "core/repository.h"
#include "core/StagingIndex.h"
#include "core/UserConfig.h"
using namespace std;

void configCommandExe(int argc, char *argv[])
{
    if (argc >= 3)
    {
        if (argv[2] == "--global")
        {
            if (argc == 4 && argv[3] == "--list")
            {
                // list all global configurations
                cout << "user.name: " << UserConfig::getGlobalName() << endl;
                cout << "user.email: " << UserConfig::getGlobalEmail() << endl;
            }
            if (argc == 5)
            {
                if (argv[3] == "user.name" && argc == 5)
                {
                    UserConfig::setGlobalName(argv[4]);
                }
                else if (argv[3] == "user.email" && argc == 5)
                {
                    UserConfig::setGlobalEmail(argv[4]);
                }
            }
        }
        else
        {
            if (argc == 4)
            {
                if (argv[2] == "--local" && argv[3] == "--list")
                {
                    // list all local configurations
                    cout << "user.name: " << UserConfig::getLocalName() << endl;
                    cout << "user.email: " << UserConfig::getLocalEmail() << endl;
                }
                else if (argv[2] == "user.name")
                {
                    // set local user name
                    UserConfig::setLocalName(argv[3]);
                }
                else if (argv[2] == "user.email")
                {
                    // set local user email
                    UserConfig::setLocalEmail(argv[3]);
                }
            }
        }
    }
    else
    {
        cout << "Invalid Params" << endl;
    }
}