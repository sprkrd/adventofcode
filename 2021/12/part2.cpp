#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

#define PRINT_PATH 0

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

#if PRINT_PATH
int number_of_paths_aux(const Graph& graph, const string& start, const string& end,
                        unordered_set<string>& visited, bool small_visited_twice, vector<string>& path) {
#else
int number_of_paths_aux(const Graph& graph, const string& start, const string& end,
                        unordered_set<string>& visited, bool small_visited_twice) {
#endif
    int npaths = 0;
    bool repeated = false;
    if (start == end) {
        npaths = 1;
#if PRINT_PATH
        for (const auto& node : path) {
            cout << node << ", ";
        }
        cout << "end" << endl;
#endif
    }
    else if (!is_small_cave(start) || !(repeated=!visited.insert(start).second) || (!small_visited_twice && start != "start")) {
#if PRINT_PATH
        path.push_back(start);
#endif
        small_visited_twice = repeated || small_visited_twice;
        for (const string& neigh : graph.find(start)->second) {
#if PRINT_PATH
            npaths += number_of_paths_aux(graph, neigh, end, visited, small_visited_twice, path);
#else
            npaths += number_of_paths_aux(graph, neigh, end, visited, small_visited_twice);
#endif
        }
        if (!repeated)
            visited.erase(start);
#if PRINT_PATH
        path.pop_back();
#endif
    }
    return npaths;
}

int number_of_paths(const Graph& graph, const string& start = "start", const string& end = "end") {
    unordered_set<string> visited;
#if PRINT_PATH
    vector<string> path;
    return number_of_paths_aux(graph, start, end, visited, false, path);
#else
    return number_of_paths_aux(graph, start, end, visited, false);
#endif
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
