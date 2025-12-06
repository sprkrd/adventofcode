#include <bits/stdc++.h>
using namespace std;

typedef uint64_t UInt;

UInt DoOp(const vector<UInt>& operands, char op)
{
    auto opFun = op=='+'? [](UInt acc, UInt n) { return acc + n; } :
                          [](UInt acc, UInt n) { return acc * n; };
    bool first = true;
    UInt ans = 0;
    for (UInt n : operands)
    {
        ans = first? n : opFun(ans, n);
        first = false;
    }
    return ans;
}

vector<UInt> DoOps(const vector<vector<UInt>>& operands, const vector<char>& operators)
{
    vector<UInt> results(operands.size());
    for (size_t i = 0; i < results.size(); ++i)
    {
        results[i] = DoOp(operands[i], operators[i]);
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
    
    string opLine = move(lines.back());
    lines.pop_back();
    
    vector<vector<UInt>> operands(1);
    for (size_t i = 0; i < lines[0].size(); ++i)
    {
        UInt number = 0;
        for (auto& row : lines)
        {
            if (row[i] != ' ')
            {
                number = number*10 + (row[i]-'0');
            }
        }
        if (number == 0)
        {
            operands.emplace_back();
        }
        else
        {
            operands.back().push_back(number);
        }
    }
    
    vector<char> operators;
    istringstream opIn(opLine);
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
