#include "core/gitObject.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include "utils/mysha1.h"
#include "utils/myparser.h"
using namespace std;

// Global Funcion Implementation
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
string GitObjectTypetos(GitObjectType gObj)
{
    if (gObj == Tree)
        return "tree";
    if (gObj == Commit)
        return "commit";
    if (gObj == Blob)
        return "blob";
    if (gObj == Tag)
        return "tag";
    return "";
}

#pragma region GitObject
// GitObject Constructors Implementation

GitObject::GitObject(GitObjectType objType, string contents)
{
    this->objectType = objType;
    this->contents = contents;
}

GitObject::GitObject(string serializedObject) // deserialize object
{
    string header = serializedObject.substr(0, serializedObject.find('\0'));
    string contents = serializedObject.substr(serializedObject.find('\0') + 1);

    vector<string> parts = split(header, '\n');
    objectType = stoGitObjectType(parts[0]);
}
// GitObject Funcitions Implementation
string GitObject::getHash()
{
    string header = to_string(objectType) + " " + to_string(contents.size()) + "\0";
    string store = header + contents;

    string hash = sha1(store);
    return hash;
}

string GitObject::serialize()
{
    string header = GitObjectTypetos(objectType) + " " + to_string(contents.size()) + '\0';
    string headerContent = header + contents;
    return headerContent;
}

#pragma endregion

#pragma region BlobObject
// BlobObjects Constructors Implementation
BlobObject::BlobObject(string filename, string contents) : GitObject(Blob, contents) {}
BlobObject::BlobObject(string serializedObject) : GitObject(serializedObject) {}

#pragma endregion

#pragma region CommitObject

// CommitObject Constructors Implementation
CommitObject::CommitObject(string treeHash, vector<string> parentHash, string author, string message, string timeStamp) : GitObject(Commit, "")
{
    this->treeHash = treeHash;
    this->parentHash = parentHash;
    this->author = author;
    this->message = message;
    this->timeStamp = timeStamp;
    this->contents = serializeContent();
}
CommitObject::CommitObject(string serializedObject) : GitObject(serializedObject) // desterilize contents
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

string CommitObject::serializeContent()
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

#pragma endregion

#pragma region TreeObject
// TreeObject Constructors Implementation

TreeObject::TreeObject() : GitObject(Tree, "")
{
}
TreeObject::TreeObject(string serilizedObject) : GitObject(serilizedObject) // desterilieze (contents to TreeObject)
{
    vector<string> lines = split(contents, '\n');
    for (auto l : lines)
    {
        vector<string> parts = split(l, '\n');
        entires.push_back(treeEntry(parts[0], stoGitObjectType(parts[1]), parts[2], parts[3]));
    }
}

string TreeObject::serializeContent()
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
void TreeObject::addEntry(treeEntry entry)
{
    entires.push_back(entry);
    contents = serializeContent();
}

#pragma endregion