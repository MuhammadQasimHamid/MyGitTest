#include "commands/checkout.h"
#include <iostream>
using namespace std;

void checkoutCommandExe(int argc, char *argv[])
{
    if (argc == 3)
    {
        cout << "You are now on " << argv[3 - 1];
    }
    else
    {
        cout << "Invalid Params for checkout command." << endl;
    }
}
