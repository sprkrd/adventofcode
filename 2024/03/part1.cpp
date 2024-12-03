#include <iostream>
#include <sstream>
#include <vector>
#include <regex>
using namespace std;

regex pattern("mul\\((\\d{1,3}),(\\d{1,3})\\)");

int main() {
    int ans = 0;
    string line;
    while (getline(cin, line))
    {
        auto begin = sregex_iterator(line.begin(), line.end(), pattern);
        auto end = sregex_iterator();
        
        for (auto i = begin; i != end; ++i)
        {
            auto match = *i;
            int a = stoi(match[1]);
            int b = stoi(match[2]);
            ans += a*b;
        }
    }
    cout << ans << endl;
}
