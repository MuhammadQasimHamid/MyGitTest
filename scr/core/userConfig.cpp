#include "core/userConfig.h"
#include "core/repository.h"

path UserConfig::localconfigFilePath;
path UserConfig::globalconfigFilePath;

// Funcs Implementation
void UserConfig::InitializeClass()
{
    localconfigFilePath = Repository::pitFolderPath / "config";
    globalconfigFilePath = std::filesystem::path("C:/Users/Public/pitconfig");
    if (!exists(globalconfigFilePath))
    {
        string fileContents = "name =example\nemail =example@gmail.com\n";
        writeFile(globalconfigFilePath, fileContents);
    }
}
void UserConfig::InitLocalConfigFile()
{
    string fileContents = "name =\nemail =\n";
    writeFile(localconfigFilePath, fileContents);
}
void UserConfig::setLocalName(const std::string &name)
{
    std::string fileContents = readFile(localconfigFilePath);
    std::vector<std::string> lines = split(fileContents, '\n');
    if (lines.size() == 2 && lines[0].find("name =") == 0)
    {
        lines[0] = "name =" + name;
    }
    std::string newFileContents = "";
    for (const auto &line : lines)
        newFileContents += line + "\n";
    writeFile(localconfigFilePath, newFileContents);
}
void UserConfig::setLocalEmail(const std::string &email)
{
    std::string fileContents = readFile(localconfigFilePath);
    std::vector<std::string> lines = split(fileContents, '\n');
    if (lines.size() == 2 && lines[1].find("email =") == 0)
    {
        lines[1] = "email =" + email;
    }
    std::string newFileContents = "";
    for (const auto &line : lines)
        newFileContents += line + "\n";
    writeFile(localconfigFilePath, newFileContents);
}

std::string UserConfig::getLocalName()
{
    std::string fileContents = readFile(localconfigFilePath);
    std::vector<std::string> lines = split(fileContents, '\n');
    if (lines.size() == 2 && lines[0].find("name =") == 0)
    {
        return lines[0].substr(6); // length of "name = "
    }
    return "";
}
std::string UserConfig::getLocalEmail()
{
    std::string fileContents = readFile(localconfigFilePath);
    std::vector<std::string> lines = split(fileContents, '\n');
    if (lines.size() == 2 && lines[1].find("email =") == 0)
    {
        return lines[1].substr(7); // length of "email = "
    }
    return "";
}
void UserConfig::setGlobalName(const std::string &name)
{
    std::string fileContents = readFile(globalconfigFilePath);
    std::vector<std::string> lines = split(fileContents, '\n');
    if (lines.size() == 2 && lines[0].find("name =") == 0)
    {
        lines[0] = "name =" + name;
    }
    std::string newFileContents = "";
    for (const auto &line : lines)
        newFileContents += line + "\n";
    writeFile(globalconfigFilePath, newFileContents);
}
void UserConfig::setGlobalEmail(const std::string &email)
{
    std::string fileContents = readFile(globalconfigFilePath);
    std::vector<std::string> lines = split(fileContents, '\n');
    if (lines.size() == 2 && lines[1].find("email =") == 0)
    {
        lines[1] = "email =" + email;
    }
    std::string newFileContents = "";
    for (const auto &line : lines)
        newFileContents += line + "\n";
    writeFile(globalconfigFilePath, newFileContents);
}

std::string UserConfig::getGlobalName()
{
    std::string fileContents = readFile(globalconfigFilePath);
    std::vector<std::string> lines = split(fileContents, '\n');
    if (lines.size() == 2 && lines[0].find("name =") == 0)
    {
        return lines[0].substr(6); // length of "name = "
    }
    return "";
}
std::string UserConfig::getGlobalEmail()
{
    std::string fileContents = readFile(globalconfigFilePath);
    std::vector<std::string> lines = split(fileContents, '\n');
    if (lines.size() == 2 && lines[1].find("email =") == 0)
    {
        return lines[1].substr(7); // length of "email = "
    }
    return "";
}

std::string UserConfig::getName()
{
    string localName = getLocalName();
    return localName == "" ? getGlobalName() : localName;
}
std::string UserConfig::getEmail()
{
    string localEmail = getLocalEmail();
    return localEmail == "" ? getGlobalEmail() : localEmail;
}
