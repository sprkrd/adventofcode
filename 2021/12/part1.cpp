#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

typedef unordered_map<string,vector<string>> Graph;

bool is_small_cave(const string& label) {
    return 'a' <= label[0] && label[0] <= 'z';
}

vector<string> split(const string& str, const string& delim=" \n\t") {
    vector<string> parts;
    size_t pos = 0;
    while (pos < str.length()) {
        size_t pos_next = str.find_first_of(delim, pos);
        if (pos_next > pos)
            parts.push_back(str.substr(pos, pos_next-pos));
        pos = pos_next == string::npos? string::npos : pos_next + 1;
    }
    return parts;
}

int number_of_paths_aux(const Graph& graph, const string& start, const string& end, unordered_set<string>& visited) {
    int npaths = 0;
    if (start == end)
        npaths = 1;
    else if (!is_small_cave(start) || !visited.count(start)) {
        visited.insert(start); // alternatively, add only small caves to this set. It doesn't make a difference.
        for (const string& neigh : graph.find(start)->second) {
            npaths += number_of_paths_aux(graph, neigh, end, visited);
        }
        visited.erase(start);
    }
    return npaths;
}

int number_of_paths(const Graph& graph, const string& start = "start", const string& end = "end") {
    unordered_set<string> visited;
    return number_of_paths_aux(graph, start, end, visited);
}

int main() {
    Graph graph;
    
    string line;
    while (getline(cin, line)) {
        auto parts = split(line, "-");
        graph[parts[0]].push_back(parts[1]);
        graph[parts[1]].push_back(parts[0]);
    }
    
    cout << number_of_paths(graph) << endl;
}
