#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#include <string>
#include <vector>
#include <iostream>

namespace json
{

    std::string escape(const std::string &s);

    void printString(const std::string &key, const std::string &value, bool comma = true);
    void printArray(const std::string &key, const std::vector<std::string> &values, bool comma = true);
    void beginObject();
    void endObject(bool comma = false);
    void beginArray();
    void endArray(bool comma = false);

}

#endif
