#include<iostream>
#include<fstream>
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

};