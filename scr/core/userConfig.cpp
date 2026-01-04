#include "core/userConfig.h"
#include "core/repository.h"

void UserConfig::InitializeClass()
{
    localconfigFilePath = Repository::project_absolute / "config";
    globalconfigFilePath = std::filesystem::path(std::getenv("HOME")) / ".pitconfig";
    string fileContents = "name =\nemail = \n";
    if (!std::filesystem::exists(localconfigFilePath))
    {
        writeFile(localconfigFilePath, fileContents);
    }
}
void UserConfig::setLocalName(const std::string &name)
{
    std::string fileContents = readFile(localconfigFilePath);
    std::vector<std::string> lines = split(fileContents, '\n');
    if (lines.size() == 2 && lines[0].find("name =") == 0)
    {
        lines[0] = "name = " + name;
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
        lines[1] = "email = " + email;
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
    if (lines.size() == 2 && lines[1].find("email =") == 0)
    {
        return lines[0].substr(7); // length of "name = "
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
        lines[0] = "name = " + name;
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
        lines[1] = "email = " + email;
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
        return lines[0].substr(7); // length of "name = "
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
