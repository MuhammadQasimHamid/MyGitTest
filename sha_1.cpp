#ifndef sha_1
#define sha_1

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <openssl/sha.h>


std::string sha1(const std::string &input)
{
    unsigned char hash[SHA_DIGEST_LENGTH];

    // Compute the SHA-1 hash
    SHA1(reinterpret_cast<const unsigned char *>(input.c_str()), input.length(), hash);

    // Convert binary hash to a hexadecimal string
    std::stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(hash[i]);
    }
    return ss.str();
}

#endif