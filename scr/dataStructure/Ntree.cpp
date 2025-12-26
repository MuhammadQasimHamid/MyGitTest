#include <iostream>
#include <vector>
#include "dataStructure/Ntree.h"
#include "core/StagingIndex.h"
#include "utils/myparser.h"
using namespace std;

TreeNode::TreeNode(string name, bool isFile)
{
    this->isFile = isFile;
    this->name = name;
}

NTree::NTree()
{
    this->root = new TreeNode("\\", false);
}

TreeNode *NTree::add(const indexEntry &entry)
{
    vector<string> parts = split(entry.path, '\\');
    TreeNode *current = root;

    for (size_t i = 0; i < parts.size(); i++)
    {
        const string &part = parts[i];
        bool isLast = (i == parts.size() - 1);

        TreeNode *next = nullptr;

        for (auto *child : current->children)
        {
            if (child->name == part)
            {
                next = child;
                break;
            }
        }

        if (!next)
        {
            next = new TreeNode(part, isLast);
            next->objectType = isLast ? Blob : Tree;
            current->children.push_back(next);
        }

        current = next;

        if (isLast)
        {
            current->isFile = true;
            current->hash = entry.hash;
            current->mode = entry.mode;
            current->objectType = Blob;
        }
    }

    return root;
}
