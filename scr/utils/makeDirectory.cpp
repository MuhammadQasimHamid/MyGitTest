#include "utils/makeDirectory.h"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <filesystem>
#include <stdexcept>
using namespace std;
using namespace std::filesystem;

void makeDirectory(const char *dirName)
{
    path absolutePath = absolute(".");
    path directoryPath = absolutePath / dirName;

    if (!exists(directoryPath))
    {
        create_directory(directoryPath);
        cout << "Directory created: " << directoryPath
             << endl;
    }
    else
    {
        throw runtime_error("Already pit has initialized in current directory.");
    }
    return;
}
