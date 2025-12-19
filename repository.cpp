#include<iostream>
#include<filesystem>
// #include<ostream>
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
    void initRepo()
    {
        try
        {

            fs::create_directory(".mygit");
            fs::create_directory(".mygit/objects");
            fs::create_directory(".mygit/refs");
            fs::create_directory(".mygit/refs/heads");
            // fs::create_directory(".mygit/logs");            

        }
        catch(fs::filesystem_error e)
        {
            cout << "Error: " << e.what() << endl;
        }    
    }
};