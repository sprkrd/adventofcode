#include <bits/stdc++.h>
using namespace std;

typedef int64_t Int;
typedef uint64_t UInt;

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

struct Row
{
    UInt target;
    vector<UInt> buttons;
    vector<UInt> joltages;
};

UInt TargetToUInt(const string& target)
{
    UInt ans = 0;
    for (auto it = target.rbegin(); it < target.rend(); ++it)
    {
        ans = ans*2 + (*it=='#');
    }
    return ans;
}

UInt ButtonToUInt(const string& button)
{
    UInt ans = 0;
    for (const auto& wire : Split(button,","))
    {
        ans |= 1ULL << stoi(wire);
    }
    return ans;
}

vector<Row> Input()
{
    vector<Row> input;
    string line;
    while (getline(cin,line))
    {
        Row row;
        auto parts = Split(line);
        row.target = TargetToUInt(parts[0].substr(1,parts[0].size()-2));
        
        for (size_t i = 1; i < parts.size()-1; ++i)
        {
            row.buttons.push_back(ButtonToUInt(parts[i].substr(1,parts[i].size()-2)));
        }
        
        
        for (const auto& joltsStr : Split(parts.back().substr(1,parts.back().size()-2), ","))
        {
            row.joltages.push_back(stoull(joltsStr));
        }
        input.push_back(move(row));
    }
    return input;
}

uint64_t NextCombination(uint64_t combination)
{
    int tz = __builtin_ctzll(combination);
    combination >>= tz;
    combination += 1;
    int tz2 = __builtin_ctzll(combination);
    combination <<= tz;
    combination |= (1ULL<<(tz2-1))-1;
    return combination;
}

UInt MostSignificantBit(UInt n)
{
    return 8*sizeof(UInt) - __builtin_clzll(n) - 1;
}

UInt GetCombination(const vector<UInt>& buttons, UInt mask)
{
    UInt ans = 0;
    for (size_t i = 0; i < buttons.size(); ++i)
    {
        if ((mask>>i)&1) ans ^= buttons[i];
    }
    return ans;
}

UInt Solve(const Row& row)
{
    if (!row.target) return 0;
    for (size_t i = 1; i <= row.buttons.size(); ++i)
    {
        UInt mask = (1ULL << i)-1;
        while (MostSignificantBit(mask) < row.buttons.size())
        {
            auto comb = GetCombination(row.buttons, mask);
            if (comb == row.target)
            {
                return i;
            }
            mask = NextCombination(mask);
        }
    }
    return 0;
}

int main()
{
    auto input = Input();
    UInt ans = 0;
    for (const auto& row : input)
    {
        ans += Solve(row);
    }
    cout << ans << endl;
}
