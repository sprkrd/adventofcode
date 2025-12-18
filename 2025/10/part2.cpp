#include <bits/stdc++.h>
using namespace std;

typedef int64_t Int;
typedef uint64_t UInt;
typedef array<Int,2> Point2i;

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

vector<Row> Input()
{
    vector<Row> input;
    string line;
    while (getline(cin,line))
    {
        Row row;
        auto parts = Split(line);
        row.target = bitset<64>(parts[0].substr(1,parts[0].size()-2), '.', '#').to_ullong();
        
        for (const auto& idxStr : Split(parts[1].substr(1,parts[1].size()-2), ","))
        {
            row.buttons.push_back(stoull(idxStr));
        }
        
        for (const auto& joltsStr : Split(parts[2].substr(1,parts[2].size()-2), ","))
        {
            row.joltages.push_back(stoull(joltsStr));
        }
        input.push_back(move(row));
    }
    return input;
}

int main()
{
    auto input = Input();
    
    for (const auto& row : input)
    {
        cout << row.target << " (" << row.buttons[0];
        for (size_t i = 1; i < row.buttons.size(); ++i)
        {
            cout << ',' << row.buttons[i];
        }
        cout << ") {" << row.joltages[0];
        for (size_t i = 1; i < row.joltages.size(); ++i)
        {
            cout << ',' << row.joltages[i];
        }
        cout << "}" << endl;
    }
}
