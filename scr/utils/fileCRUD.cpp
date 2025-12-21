#include "utils/fileCRUD.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <filesystem>
using namespace std;

void writeFile(const filesystem::path &filePath, const string &content)
{
    ofstream file(filePath);
    if (!file.is_open())
    {
        throw runtime_error(string("Failed to create File: ") + filePath.string());
    }
    file << content;
    file.close();
}


string readFile(const filesystem::path &filePath)
{
    string contents = "";
    string line;
    cout << "read file" << endl;
    ifstream file(filePath);
    if (file.is_open())
    {
        cout << "is open" << endl;
        while (!file.eof())
        {
            cout << "while" << endl;
            getline(file, line);
            contents += line + "\n";
        }
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
