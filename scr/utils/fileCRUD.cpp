#include "utils/fileCRUD.h"
#include "core/repository.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <filesystem>
using namespace std;
namespace fs = filesystem;
void writeFile(const filesystem::path &filePath, const string &content)
{
    ofstream file(filePath);
    if (file.is_open())
    {
        file << content;
    }
    else
    {
        if (!(fs::exists(filePath)))
        {
            cout << "Error: File not found!" << endl;
        }
        else
        {
            cout << "Error while opening file" << endl;
        }
    }
    file.close();
}

string readFile(const filesystem::path &filePath)
{
    string contents = "";
    string line;
    ifstream file(filePath);
    if (file.is_open())
    {
        while (!file.eof())
        {
            getline(file, line);
            contents += line;
            if (!file.eof())
                contents += "\n";
        }
    }
    else
    {
        cout << "Error: " << filePath << " doesn't exist" << endl;
    }
    file.close();
    return contents;
}
string readFileWithStoredObjectHash(const string &hash)
{
    fs::path filePath = Repository::objectsFolderPath / hash.substr(0, 2) / hash.substr(2);
    string contents = "";
    string line;
    ifstream file(filePath);
    if (file.is_open())
    {
        while (!file.eof())
        {
            getline(file, line);
            contents += line;
            if (!file.eof())
                contents += "\n";
        }
    }
    else
    {
        cout << "Error: " << filePath << " doesn't exist" << endl;
    }
    file.close();
    return contents;
}

void writeFileWithBytes(const string &path, const vector<unsigned char> &data)
{
    // Open in binary mode!
    ofstream file(path, ios::binary);
    if (!file)
        throw runtime_error("Could not open file");

    file.write(reinterpret_cast<const char *>(data.data()), data.size());
    file.close();
}
