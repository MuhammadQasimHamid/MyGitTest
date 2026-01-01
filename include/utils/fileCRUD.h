#ifndef FILECRUD_H
#define FILECRUD_H
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <filesystem>


void writeFile(const std::filesystem::path &filePath, const std::string &content = "");

std::string readFile(const std::filesystem::path &filePath);
std::string readFileWithStoredObjectHash(const std::string &hash);
void writeFileWithBytes(const std::string &path, const std::vector<unsigned char> &data);
void deleteFile(const std::filesystem::path filePath);
bool is_a_subfolder( std::filesystem::path base,std::filesystem::path child);

bool is_subpath(const std::filesystem::path& parent, const std::filesystem::path& child);
std::filesystem::path normalize_relative(const std::filesystem::path &p);
#endif