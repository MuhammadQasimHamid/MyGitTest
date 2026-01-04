#include "utils/mytime.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <chrono>

long long getEpochSeconds()
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(
               now.time_since_epoch())
        .count();
}
std::string epochToString(long long epoch)
{
    return std::to_string(epoch);
}