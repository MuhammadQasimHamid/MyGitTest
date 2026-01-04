#ifndef USERCONFIG_H
#define USERCONFIG_H

#include <iostream>
#include <filesystem>

class UserConfig
{
public:
    static std::filesystem::path localconfigFilePath;
    static std::filesystem::path globalconfigFilePath;
    static void InitializeClass();

    static void setLocalName(const std::string &name);
    static void setLocalEmail(const std::string &email);
    static std::string getLocalName();
    static std::string getLocalEmail();

    static void setGlobalName(const std::string &name);
    static void setGlobalEmail(const std::string &email);
    static std::string getGlobalName();
    static std::string getGlobalEmail();
};

#endif