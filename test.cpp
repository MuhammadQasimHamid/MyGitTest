#include <iostream>
#include "opensslLocal/sha.h"

int main()
{
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned char *)"test", 4, hash);
    std::cout << "OpenSSL SHA-1 working\n";
    return 0;
}
