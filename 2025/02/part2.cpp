#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <numeric>
#include <unordered_set>
using namespace std;

typedef pair<uint64_t, uint64_t> Range;

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

unsigned GetNumberOfDigits(uint64_t n)
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

uint64_t GetTenthPower(unsigned power)
{
    uint64_t ans = 1;
    for (unsigned i = 0; i < power; ++i) ans*=10;
    return ans;
}

uint64_t Repeat(uint64_t n, unsigned times)
{
    unsigned nDigits = GetNumberOfDigits(n);
    uint64_t tenthPower = GetTenthPower(nDigits);
    uint64_t ans = 0;
    for (unsigned i = 0; i < times; ++i) ans = ans*tenthPower + n;
    return ans;
}

uint64_t SumInvalidIds(const vector<Range>& ranges)
{
    unordered_set<uint64_t> invalidIds;
    for (uint64_t i = 1; Repeat(i, 2) <= ranges.back().second; ++i)
    {
        for (unsigned r = 2; Repeat(i, r) <= ranges.back().second; ++r)
        {
            uint64_t id = Repeat(i, r);
            auto rangeIt = lower_bound(ranges.begin(), ranges.end(), id, [](const Range& range, uint64_t i)
            {
                return i > range.second;
            });
            if (rangeIt != ranges.end() && rangeIt->first <= id)
            {
                invalidIds.insert(id);
            }
        }
    }
    return accumulate(invalidIds.begin(), invalidIds.end(), 0ULL);
}

int main()
{
    vector<Range> ranges;
    string line;
    while (getline(cin, line, ','))
    {
        auto parts = Split(line, "-");
        ranges.emplace_back(stoull(parts[0]), stoull(parts[1]));
    }
    sort(ranges.begin(), ranges.end());
    cout << ranges.size() << endl;
    cout << SumInvalidIds(ranges) << endl;
}
