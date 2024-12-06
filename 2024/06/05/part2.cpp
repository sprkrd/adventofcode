#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
#include <stack>
#include <string>
using namespace std;

typedef vector<int> Rule;
typedef vector<Rule> DirectedGraph;
typedef vector<int> Update;
typedef vector<Update> UpdateList;

vector<string> split(const string& str, const string& delim=" \n\t") {
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

DirectedGraph parseDirectedGraph(istream& in)
{
    DirectedGraph directedGraph;
    string line;
    while (getline(cin, line) && !line.empty())
    {
        auto parts = split(line, "|");
        int lhs = stoi(parts[0]);
        int rhs = stoi(parts[1]);
        if (directedGraph.size() <= lhs)
        {
            directedGraph.resize(lhs+1);
        }
        directedGraph[lhs].push_back(rhs);
    }
    return directedGraph;
}

UpdateList parseUpdateList(istream& in)
{
    UpdateList updateList;
    string line;
    while (getline(cin, line) && !line.empty())
    {
        auto parts = split(line, ",");
        Update update(parts.size());
        for (unsigned i = 0; i < parts.size(); ++i)
        {
            update[i] = stoi(parts[i]);
        }
        updateList.push_back(move(update));
    }
    return updateList;
}

Update topologicalSort(const DirectedGraph& directedGraph, const Update& graphSubset)
{
    int numberOfNodes = directedGraph.size();
    vector<bool> isInSubset(numberOfNodes);;
    for (int node0 : graphSubset)
    {
        isInSubset[node0] = true;
    }
    vector<bool> marked(numberOfNodes);
    Update sortedEntries;
    for (int node0 : graphSubset)
    {
        vector<bool> currentPath(numberOfNodes);
        stack<pair<int,bool>> stk;
        stk.emplace(node0, false);
        while (!stk.empty())
        {
            auto[node,expanded] = stk.top();
            stk.pop();
            if (marked[node] || !isInSubset[node])
            {
                continue;
            }
            if (expanded)
            {
                currentPath[node] = false;
                marked[node] = true;
                sortedEntries.push_back(node);
            }
            else
            {
                stk.emplace(node, true);
                assert(!currentPath[node]);
                currentPath[node] = true;
                for (int preceded : directedGraph[node])
                {
                    stk.emplace(preceded, false);
                }
            }
        }
    }
    reverse(sortedEntries.begin(), sortedEntries.end());
    return sortedEntries;
}

bool isValid(const DirectedGraph& directedGraph, const Update& update)
{
    vector<bool> seenEntry(directedGraph.size());
    for (int entry : update)
    {
        for (int preceded : directedGraph[entry])
        {
            if (seenEntry[preceded])
            {
                return false;
            }
        }
        seenEntry[entry] = true;
    }
    return true;
}

int main() {
    DirectedGraph directedGraph = parseDirectedGraph(cin);
    UpdateList updateList = parseUpdateList(cin);
    int ans = 0;
    for (const auto& update : updateList)
    {
        if (!isValid(directedGraph, update))
        {
            Update sortedEntries = topologicalSort(directedGraph, update);
            ans += sortedEntries[sortedEntries.size()/2];
        }
    }
    cout << ans << endl;
}
