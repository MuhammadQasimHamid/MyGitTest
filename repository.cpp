#include <iostream>
#include <fstream>
#include <filesystem>
using namespace std::filesystem;
class Repository
{
public:
    path project_absolute = absolute(".");
    path pitFolderPath = project_absolute / ".pit";
    path objectsFolderPath = pitFolderPath / "objects";
    path refsFolderPath = pitFolderPath / "refs";
    path refsHeadFolderPath = refsFolderPath / "heads";
    path HEADFilePath = pitFolderPath / "HEAD";
    path indexFilePath = pitFolderPath / "index";

    Repository()
    {
    }
    bool initRepo() // returns true if repo initialized successfully
    {
        try
        {

            if (!exists(pitFolderPath))
            {
                // Directories
                create_directory(pitFolderPath);
                // .git/objects
                create_directory(objectsFolderPath);
                // .git/refs
                create_directory(refsFolderPath);
                // .git/refs/heads
                create_directory(refsHeadFolderPath);

                // Files
                //  .git/HEAD
                ofstream headFile(HEADFilePath); // this file stores pointer to branches
                if (headFile.is_open())
                {
                    // stores current branch in /HEAD
                    headFile << "ref: refs/heads/main\n";
                }
                headFile.close();
                // .git/index
                ofstream indexFile(indexFilePath);
                indexFile.close();
            }
            else
            {
                throw runtime_error("Already pit has initialized in current directory.");
            }

            cout << "Initialized empty pit repository." << endl;
            return true;
        }
        catch (const exception &e)
        {
            cerr << e.what() << '\n';
        }
        catch (...)
        {
            cerr << "Their is any error occurs during creating repository." << endl;
        }
        return false;
    }

    void storeObject()
    {
    }
};