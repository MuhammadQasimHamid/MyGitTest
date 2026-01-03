#ifndef NTree_H
#define NTree_H

#include <iostream>
#include <functional>
#include "core/gitObject.h"
#include "core/StagingIndex.h"
#include <vector>
#include <algorithm>
#include <cctype>
using namespace std;

// Helper functions: centralize platform-specific name handling and make the
// traversal checks simple and easy to read.
static inline std::string toLowerCopy(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

static inline bool isSkippedDirName(const std::string &name)
{
#if defined(_WIN32)
    std::string n = toLowerCopy(name);
    return n == ".git" || n == ".pit" || n == "build" || n == ".vscode";
#else
    return name == ".git" || name == ".pit" || name == "build" || name == ".vscode";
#endif
}

static inline bool relPathContainsSkipped(const fs::path &p)
{
    for (auto &part : p)
    {
        if (isSkippedDirName(part.string()))
            return true;
    }
    return false;
} 

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

static inline void traverseDirectory(
    const fs::path &currentPath,
    const std::function<void(const fs::path &)> &fileCallback)
{
    for (const auto &entry : fs::directory_iterator(currentPath,
                                                    fs::directory_options::skip_permission_denied))
    {
        fs::path path = entry.path();
        std::string filename = path.filename().string();

        // Skip hidden metadata directories and avoid recursing into them.
        if (entry.is_directory() && isSkippedDirName(filename))
        {
            continue; // skip this directory completely
        }

        if (entry.is_directory())
        {
            // Recurse into subdirectory
            traverseDirectory(path, fileCallback);
        }
        else if (entry.is_regular_file())
        {
            // Normalize path relative to repository
            fs::path relPath = fs::relative(path, Repository::project_absolute).lexically_normal();

            // Ensure we are not reporting files inside skipped directories (e.g., build)
            if (relPathContainsSkipped(relPath))
                continue;

            fileCallback(relPath);
        }
    }
}

#endif
