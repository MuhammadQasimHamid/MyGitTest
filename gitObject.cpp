#include <iostream>
#include <fstream>
#include <vector>
#include <string>
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
    string contents;
    GitObject(GitObjectType objType, string contents)
    {
        this->objectType = objType;
        this->contents = contents;
    }
    GitObject() {}
    string getHash()
    {
        string header = to_string(objectType) + " " + to_string(contents.size()) + "\0";
        string store = header + contents;

        string hash = sha1(store);
        return hash;
    }
};

class BlobObject : GitObject
{
public:
    BlobObject(string contents) : GitObject(Blob, contents) {}
};

class CommitObject : GitObject
{
public:
    string treeHash;
    vector<string> parentHash;
    string author;
    string message;
    string timeStamp;
    CommitObject(string treeHash, vector<string> parentHash, string author, string message, string timeStamp) : GitObject(Commit, "")
    {
        this->treeHash = treeHash;
        this->parentHash = parentHash;
        this->author = author;
        this->message = message;
        this->timeStamp = timeStamp;
        this->contents = serializeContent();
    }

    string serializeContent()
    {
        string res = "";
        res += "tree " + this->treeHash + "\n";
        for (auto hs : parentHash)
            res += "parent " + hs + "\n";
        res += "author " + this->author + " " + this->timeStamp + " +0000" + "\n";
        res += "\n";
        res += message + "\n";
        return res;
    }
};