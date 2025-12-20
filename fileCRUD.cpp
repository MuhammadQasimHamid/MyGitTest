#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
using namespace std;
void makeFile(const string &fileName)
{
    ofstream file(fileName);
    if (!file.is_open())
    {
        throw runtime_error("Error: Could not open the file.");
    }
}

void writeFile(const string &fileName, const string &content)
{
    ofstream file(fileName);
    if (!file.is_open())
    {
        throw runtime_error(string("Failed to create File: ") + fileName);
    }
    file << content << endl;
    file.close();
}