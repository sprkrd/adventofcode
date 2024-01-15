#include <cassert>
#include <iostream>
#include <regex>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

typedef unordered_map<string,vector<string>> graph_t;

void add_adjacency(graph_t& graph, const string& adjstr) {
  static const regex re_head("^([\\s\\w]+) bags contain");
  static const regex re_tail_item("(\\d+) ([\\s\\w]+) bags?[,\\.]");
  smatch match;
  assert(regex_search(adjstr,match,re_head));
  auto item_it = sregex_iterator(adjstr.begin(), adjstr.end(), re_tail_item);
  auto item_end = sregex_iterator();
  for (; item_it != item_end; ++item_it) {
    graph[item_it->str(2)].push_back(match.str(1));
  }
}

unordered_set<string> reachable(const graph_t& graph, const string& start) {
  unordered_set<string> result;
  stack<string> s;
  s.push(start);
  while (!s.empty()) {
    string u = move(s.top());
    auto adj_it = graph.end();
    s.pop();
    bool is_new = result.insert(u).second;
    if (is_new && (adj_it=graph.find(u)) != graph.end()) {
      for (const auto& v : adj_it->second)
        s.push(v);
    }
  }
  result.erase(start);
  return result;
}

int main() {
  graph_t graph;
  string line;
  while (getline(cin,line))
    add_adjacency(graph,line);
  /*
  for (const auto&[u,adj] : graph) {
    cout << u << ':';
    for (unsigned i = 0; i < adj.size(); ++i) {
      if (i) cout << ", ";
      cout << adj[i];
    }
    cout << endl;
  }
  */
  auto reachable_colors = reachable(graph, "shiny gold");
  for (const auto& color : reachable_colors)
    cout << color << endl;
  cout << reachable_colors.size() << endl;
}

