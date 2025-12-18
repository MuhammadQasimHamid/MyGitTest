#include <iostream>
#include <fstream>
#include <openssl/sha.h>
using namespace std;

enum GitObjectType
{
    Blob,
    Tree,
    Commit,
    Tag
};

class GitObject
{
public:
    GitObjectType objectType;
    string content;
    GitObject(GitObjectType objType, string content)
    {
        this->objectType = objType;
        this->content = content;
    }
    string getHash()
    {
        string header = to_string(objectType) + " " + to_string(content.size()) + "\0";
        string store = header + content;

        string hash = sha1(store);
        return hash;
    }
};