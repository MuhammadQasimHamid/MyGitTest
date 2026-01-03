#include "utils/json_helper.h"

using namespace std;

namespace json
{

    string escape(const string &s)
    {
        string out;
        for (char c : s)
        {
            if (c == '"')
                out += "\\\"";
            else if (c == '\\')
                out += "\\\\";
            else if (c == '\n')
                out += "\\n";
            else
                out += c;
        }
        return out;
    }

    void beginObject() { cout << "{\n"; }
    void endObject(bool comma)
    {
        cout << "\n}";
        if (comma)
            cout << ",";
        cout << "\n";
    }

    void beginArray() { cout << "[\n"; }
    void endArray(bool comma)
    {
        cout << "\n]";
        if (comma)
            cout << ",";
        cout << "\n";
    }

    void printString(const string &key, const string &value, bool comma)
    {
        cout << "  \"" << key << "\": \"" << escape(value) << "\"";
        if (comma)
            cout << ",";
        cout << "\n";
    }

    void printArray(const string &key, const vector<string> &values, bool comma)
    {
        cout << "  \"" << key << "\": [";
        for (size_t i = 0; i < values.size(); i++)
        {
            cout << "\"" << escape(values[i]) << "\"";
            if (i + 1 < values.size())
                cout << ", ";
        }
        cout << "]";
        if (comma)
            cout << ",";
        cout << "\n";
    }

}
