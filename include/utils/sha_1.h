#ifndef SHA_H
#define SHA_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <openssl/sha.h>

std::string sha1(const std::string &input);

#endif