#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

vector<string> Split(const string& str, const string& delim=" \n\t") {
    vector<string> parts;
    size_t pos = 0;
    while (pos < str.length())
    {
        size_t pos_next = str.find_first_of(delim, pos);
        if (pos_next > pos)
        {
            parts.push_back(str.substr(pos, pos_next-pos));
        }
        pos = pos_next == string::npos? string::npos : pos_next + 1;
    }
    return parts;
}

unsigned GetNumberOfDigits(int64_t n)
{
    if (n == 0) return 1;
    unsigned ans = 0;
    while (n)
    {
        ++ans;
        n /= 10;
    }
    return ans;
}

int64_t NextTenthPowerWithEvenDigits(int64_t n)
{
    int64_t ans = 10;
    while (ans < n) ans *= 100;
    return ans;
}

int64_t GetFirstHalf(int64_t n)
{
    unsigned nDigits = GetNumberOfDigits(n);
    for (unsigned i = 0; i < nDigits/2; ++i) n /= 10;
    return n;
}

pair<int64_t,int64_t> SplitNumber(int64_t n)
{
    unsigned nDigits = GetNumberOfDigits(n);
    unsigned tenthPower = 1;
    for (unsigned i = 0; i < nDigits/2; ++i) tenthPower *= 10;
    return {n/tenthPower, n%tenthPower};
}

int64_t Concatenate(int64_t n, int64_t m)
{
    if (m == 0) return n*10;
    int64_t k = m;
    while (k)
    {
        n *= 10;
        k /= 10;
    }
    return n + m;
}

int64_t SumInvalidIds(int64_t init, int64_t end)
{
    if (GetNumberOfDigits(init)%2 == 1)
    {
        init = NextTenthPowerWithEvenDigits(init);
    }
    int64_t ans = 0;
    auto[firstHalf,secondHalf] = SplitNumber(init);
    if (secondHalf > firstHalf) ++firstHalf;
    int64_t invalidId;
    while ((invalidId=Concatenate(firstHalf, firstHalf)) <= end)
    {
        //cout << invalidId << endl;
        ans += invalidId;
        ++firstHalf;
    }
    return ans;
}

int main()
{
    int64_t ans = 0;
    string line;
    while (getline(cin, line, ','))
    {
        auto parts = Split(line, "-");
        int64_t initRange = stoll(parts[0]);
        int64_t endRange = stoll(parts[1]);
        ans += SumInvalidIds(initRange, endRange);
    }
    cout << ans << endl;
}
