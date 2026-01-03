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
        if (!(exists(filePath)))
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
        throw("Error: " + filePath.string() + " doesn't exist" );
    }
    file.close();
    return contents;
}
string readFileWithStoredObjectHash(const string &hash)
{
    path filePath = Repository::objectsFolderPath / hash.substr(0, 2) / hash.substr(2);
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
bool storedObjectExistsWithHash(const string &hash) 
{
    path filePath = Repository::objectsFolderPath / hash.substr(0, 2) / hash.substr(2);
    if (exists(filePath))
        return true;
    return false;
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

void deleteFile(const path filePath)
{
    std::filesystem::remove(filePath);
}

bool is_a_subfolder( path base,path child)
{
    // Normalize both paths lexically
    path normal_base = base.lexically_normal();
    path normal_child = child.lexically_normal();

    // If the base path is a file, it cannot be a subfolder of anything
    if (exists(normal_base) && !is_directory(normal_base))
    {
        return false;
    }

    // Use std::mismatch to compare the path elements
    auto mismatch_pair = std::mismatch(normal_base.begin(), normal_base.end(),
                                       normal_child.begin(), normal_child.end());

    // If the base path elements all matched the beginning of the child path elements,
    // then the child is a subfolder (or is the same path)
    bool res = mismatch_pair.first == normal_base.end();
    return res;
}

bool is_subpath(const fs::path& parent, const fs::path& child)
{
    auto p_it = parent.begin();
    auto c_it = child.begin();

    for (; p_it != parent.end() && c_it != child.end(); ++p_it, ++c_it)
    {
        if (*p_it != *c_it)
            return false;
    }
    return p_it == parent.end();
}

fs::path normalize_relative(const fs::path &p)
{
    return fs::relative(fs::absolute(p), Repository::project_absolute)
        .lexically_normal();
}