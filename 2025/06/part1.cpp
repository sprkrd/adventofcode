#include <bits/stdc++.h>
using namespace std;

typedef uint64_t UInt;

UInt DoOp(size_t i, const vector<vector<UInt>> operands, char op)
{
    auto opFun = op=='+'? [](UInt acc, UInt n) { return acc + n; } :
                          [](UInt acc, UInt n) { return acc * n; };
    bool first = true;
    UInt ans = 0;
    for (const auto& row : operands)
    {
        ans = first? row[i] : opFun(ans, row[i]);
        first = false;
    }
    return ans;
}

vector<UInt> DoOps(const vector<vector<UInt>>& operands, const vector<char>& operators)
{
    vector<UInt> results(operands[0].size());
    for (size_t i = 0; i < results.size(); ++i)
    {
        results[i] = DoOp(i, operands, operators[i]);
    }
    return results;
}

pair<vector<vector<UInt>>, vector<char>> Input()
{
    vector<string> lines;
    string line;
    while (getline(cin, line))
    {
        lines.push_back(move(line));
    }
    
    vector<vector<UInt>> operands;
    vector<char> operators;
    for (size_t i = 0; i < lines.size()-1; ++i)
    {
        istringstream in(lines[i]);
        vector<UInt> row;
        UInt number;
        while (in >> number)
        {
            row.push_back(number);
        }
        operands.push_back(move(row));
    }
    istringstream opIn(lines.back());
    char op;
    while (opIn >> op)
    {
        operators.push_back(op);
    }
    return {operands, operators};
}

int main()
{
    auto[operands,operators] = Input();
    auto results = DoOps(operands, operators);
    cout << accumulate(results.begin(), results.end(), 0ULL) << endl;
}
