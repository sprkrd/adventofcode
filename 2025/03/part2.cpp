#include <bits/stdc++.h>
using namespace std;

constexpr size_t kDigits = 12;

uint64_t MaxJoltage(const string& bank)
{
    vector<int> digits;
    size_t remainingBatteries = bank.size();
    for (char c : bank)
    {
        --remainingBatteries;
        int digit = c-'0';
        while (!digits.empty() && digit>digits.back() && remainingBatteries>=kDigits-digits.size())
        {
            digits.pop_back();
        }
        if (digits.size() < kDigits)
        {
            digits.push_back(digit);
        }
    }
    return accumulate(digits.begin(), digits.end(), 0ULL, [](uint64_t acc, int digit)
    {
        return acc*10 + digit;
    });
}

int main()
{
    uint64_t ans = 0;
    string line;
    while (getline(cin, line))
    {
        uint64_t jolts = MaxJoltage(line);
        ans += jolts;
    }
    cout << ans << endl;
}
