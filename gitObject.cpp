#include<iostream>
#include<fstream>
#include <openssl/sha.h>
using namespace std;

enum GitObjectType{
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
    GitObject(GitObjectType objType,string content)
    {
        this->objectType = objType;
        this->content = content;
    }
    string getHash()
    {
        string hashed = SHA1();
        return hashed;
    }
};