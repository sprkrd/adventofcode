#include <cassert>
#include <iostream>
#include <regex>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
using namespace std;

typedef unordered_map<string,vector<pair<string,int>>> graph_t;
typedef unordered_map<string,int> cache_t;

void add_adjacency(graph_t& graph, const string& adjstr) {
  static const regex re_head("^([\\s\\w]+) bags contain");
  static const regex re_tail_item("(\\d+) ([\\s\\w]+) bags?[,\\.]");
  smatch match;
  assert(regex_search(adjstr,match,re_head));
  auto item_it = sregex_iterator(adjstr.begin(), adjstr.end(), re_tail_item);
  auto item_end = sregex_iterator();
  auto[adj_it,_] = graph.emplace(match.str(1), graph_t::mapped_type());
  for (; item_it != item_end; ++item_it)
    adj_it->second.emplace_back(item_it->str(2), stoi(item_it->str(1)));
}

int count_bags(const graph_t& graph, const string& bag, cache_t& cache) {
  auto found = cache.find(bag);
  if (found == cache.end()) {
    auto it = graph.find(bag);
    int count = 0;
    for (const auto&[inner_bag,qty] : it->second)
      count += qty*(1 + count_bags(graph,inner_bag,cache));
    tie(found,ignore) = cache.emplace(bag, count);
  }
  return found->second;
}

int count_bags(const graph_t& graph, const string& bag) {
  cache_t cache;
  return count_bags(graph,bag,cache);
}

int main() {
  graph_t graph;
  string line;
  while (getline(cin,line))
    add_adjacency(graph,line);
  cout << count_bags(graph,"shiny gold") << endl;
  /*
  for (const auto&[u,adj] : graph) {
    cout << u << ':';
    for (unsigned i = 0; i < adj.size(); ++i) {
      cout << (i==0? " " : ", ");
      cout << get<0>(adj[i]) << " (" << get<1>(adj[i]) << ')';
    }
    cout << endl;
  }
  */
}

