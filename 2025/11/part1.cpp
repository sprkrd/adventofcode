#include <bits/stdc++.h>
using namespace std;

typedef int64_t Int;
typedef uint64_t UInt;
typedef unordered_map<string,vector<string>> Graph;

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

Graph Input()
{
    Graph input;
    string line;
    while (getline(cin, line))
    {
        auto parts = Split(line);
        string u = move(parts[0]);
        u.pop_back();
        for (size_t i = 1; i < parts.size(); ++i)
        {
            input[u].push_back(move(parts[i]));
        }
    }
    return input;
}

vector<string> TopoSort(const Graph& graph, const string& seed = "you")
{
    vector<string> sorted;
    unordered_set<string> marked;
    unordered_set<string> inPath;
    vector<pair<string,bool>> stk{{seed,false}};
    while (!stk.empty())
    {
        auto&[node, expanded] = stk.back();
        if (expanded)
        {
            sorted.push_back(node);
            marked.insert(node);
            inPath.erase(node);
            stk.pop_back();
            continue;
        }
        else if (marked.count(node))
        {
            stk.pop_back();
            continue;
        }
        assert(inPath.count(node) == 0);
        expanded = true;
        inPath.insert(std::string(node));
        auto graphEntry = graph.find(node);
        if (graphEntry != graph.end())
        {
            for (const auto& successor : graphEntry->second)
            {

                stk.emplace_back(successor, false);
            }
        }
    }
    reverse(sorted.begin(), sorted.end());
    return sorted;
}

int main()
{
    unordered_map<string, size_t> countRegister{{"you", 1}};
    Graph input = Input();
    auto sorted = TopoSort(input);
    for (const auto& node : sorted)
    {
        size_t nodeCount = countRegister[node];
        auto graphEntry = input.find(node);
        if (graphEntry != input.end())
        {
            for (const auto& successor : graphEntry->second)
            {
                countRegister[successor] += nodeCount;
            }
        }
    }
    cout << countRegister["out"] << endl;
}
