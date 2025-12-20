#ifndef FILECRUD_H
#define FILECRUD_H
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <filesystem>
using namespace std;
void makeFile(const string &fileName)
{
    ofstream file(fileName);
    if (!file.is_open())
    {
        throw runtime_error("Error: Could not open the file.");
    }
}

void writeFileWithString(const filesystem::path &filePath, const string &content)
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
    ifstream file(filePath);
    if (!file.is_open())
    {
        while (!file.eof())
        {
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

#endif