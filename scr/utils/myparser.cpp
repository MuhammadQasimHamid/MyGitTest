#include "utils/myparser.h"
#include<vector>
#include<iostream>
#include<sstream>
using namespace std;

vector<string> split(string str, char ch)
{
    istringstream ss(str);
    vector<string> parts;
    string token;
    while(getline(ss,token,ch))
    {
        parts.push_back(token);
    }
    return parts;
}
