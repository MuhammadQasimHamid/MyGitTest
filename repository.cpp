#include <iostream>
#include "makeDirectory.cpp"
#include "fileCRUD.cpp"
using namespace std;

namespace fs = filesystem;
// using namespace std;
// namespace fs = std::filesystem;
class Repository
{
public:
    string path = "";
    Repository()
    {
    }
    bool initRepo() // returns true if repo initialized successfully
    {
        try
        {
            // Directories
            makeDirectory(".pit");
            // .git/objects
            makeDirectory(".pit/objects");
            // .git/refs
            makeDirectory(".pit/refs");
            // .git/refs/heads
            makeDirectory(".pit/refs/heads");

            // Files
            //  .git/HEAD
            makeFile(".pit/HEAD"); // this file stores pointer to branches
            // .git/index
            makeFile(".pit/index");

            // stores current branch in /HEAD
            writeFile(".pit/HEAD", "ref: refs/heads/main\n");

            cout << "Initialized empty git repository." << endl;
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
};