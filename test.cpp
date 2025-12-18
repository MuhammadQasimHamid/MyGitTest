// Online C++ compiler to run C++ program online
#include <iostream>
using namespace std;
class Solution
{
public:
    bool isAlphaNum(char ch)
    {
        if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9')
            return true;
        return false;
    }
    bool isPalindrome(string s)
    {
        int l = 0, h = s.length() - 1;
        while (l < h)
        {
            cout << s[l] << s[h] << " ";
            if (!isAlphaNum(s[l]))
            {
                l++;
                continue;
            }
            if (!isAlphaNum(s[h]))
            {
                h--;
                continue;
            }
            if (s[l] != s[h])
                return false;
            l++;
            h--;
        }
        return true;
    }
};
int main()
{
    // Write C++ code here
    Solution s;
    cout << s.isPalindrome("A man, a plan, a canal: Panama");
    return 0;
}
