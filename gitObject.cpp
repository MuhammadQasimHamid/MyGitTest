#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
// #include <openssl/sha.h>
#include "sha_1.cpp"
#include "parser.cpp"
#include "compress.cpp"
using namespace std;

enum GitObjectType
{
    Blob,
    Tree,
    Commit,
    Tag,
    Nothing
};
GitObjectType stoGitObjectType(string str)
{
    if (str == "tree")
        return Tree;
    if (str == "commit")
        return Commit;
    if (str == "blob")
        return Blob;
    if (str == "tag")
        return Tag;
    return Nothing;
}
struct treeEntry
{
    string mode;
    GitObjectType type;
    string hash;
    string name;
    treeEntry(string mode, GitObjectType type, string hash, string name)
    {
        this->mode = mode;
        this->type = type;
        this->hash = hash;
        this->name = name;
    }
    void fill(string &mode, GitObjectType &type, string &hash, string &name)
    {
        mode = this->mode;
        type = this->type;
        hash = this->hash;
        name = this->name;
    }
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

    vector<unsigned char> serialize()
    {
        string header = to_string(objectType) + " " + to_string(contents.size()) + "\0";
        string headerContent = header + contents;
        return compress_git_object(headerContent);
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
    CommitObject(string contents) : GitObject(Commit, contents) // desterilize
    {
        vector<string> lines = split(contents, '\n');
        for (auto l : lines)
        {
            vector<string> parts = split(l, ' '); // divide line into parts
            string key = parts[0];                // heading of the part
            if (key == "tree")
                treeHash = parts[1];
            else if (key == "parent")
                parentHash.push_back(parts[1]);
            else if (key == "author")
            {
                author = parts[1];
                timeStamp = parts[2];
            }
            else
                message = l;
        }
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

class TreeObject : GitObject
{
    vector<treeEntry> entires;
    TreeObject() : GitObject(Tree, "")
    {
    }
    TreeObject(string contents) : GitObject(Tree, contents) // desterilieze (contents to TreeObject)
    {
        vector<string> lines = split(contents, '\n');
        for (auto l : lines)
        {
            vector<string> parts = split(l, '\n');
            entires.push_back(treeEntry(parts[0], stoGitObjectType(parts[1]), parts[2], parts[3]));
        }
    }

    string serializeContent()
    {
        string mode, hash, name;
        GitObjectType type;
        string res = "";
        for (auto e : entires)
        {
            e.fill(mode, type, hash, name); // fill variables from entry
            res += mode + " " + to_string(type) + " " + hash + " " + name + "\n";
        }
        return res;
    }
    void addEntry(treeEntry entry)
    {
        entires.push_back(entry);
    }
};
