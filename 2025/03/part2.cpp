#include <bits/stdc++.h>
using namespace std;

constexpr int kDigits = 12;

int main()
{
    uint64_t ans = 0;
    string line;
    while (getline(cin, line))
    {
        uint64_t jolts = 0;
        auto begin = line.begin();
        for (auto end = line.end()-kDigits+1; end <= line.end(); ++end)
        {
            auto digitIt = max_element(begin, end);
            jolts = jolts*10 + (*digitIt-'0');
            begin = digitIt+1;
        }
        ans += jolts;
    }
    cout << ans << endl;
}
