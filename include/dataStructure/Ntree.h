#ifndef NTree_H
#define NTree_H

#include <iostream>
#include "core/gitObject.h"
#include "core/StagingIndex.h"
#include <vector>
using namespace std;

struct TreeNode
{
    string mode;
    GitObjectType objectType;
    string hash;
    string name;
    bool isFile;
    vector<TreeNode *> children;
    TreeNode(string name, bool isFile);
};

class NTree
{

public:
    TreeNode *root;
    NTree();
    TreeNode *add(const indexEntry &entry);
};

#endif