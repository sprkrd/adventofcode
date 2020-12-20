#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>
using namespace std;

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

typedef vector<string> concat_t;
typedef vector<concat_t> rule_t;

ostream& operator<<(ostream& out, const vector<string>& symbols) {
  for (unsigned i = 0; i < symbols.size(); ++i) {
    if (i > 0) out << ' ';
    out << symbols[i];
  }
  return out;
}

ostream& operator<<(ostream& out, const rule_t& rule) {
  for (unsigned i = 0; i < rule.size(); ++i) {
    if (i > 0) out << " | ";
    out << rule[i];
  }
  return out;
}

void parse_rules_helper(const string& line, map<string,rule_t>& rules) {
  auto lr = split(line, ":");
  auto disjunction = split(lr[1], "|");
  for (const auto& concatenation : disjunction)
    rules[lr[0]].push_back(split(concatenation));
}

auto parse_rules(istream& in) {
  map<string,rule_t> rules;
  string line;
  while (getline(cin,line) && !line.empty())
    parse_rules_helper(line, rules);
  return rules;
}

vector<int> new_partition(int number_of_sets) {
  vector<int> partition(number_of_sets-1);
  for (unsigned i = 0; i < partition.size(); ++i)
    partition[i] = i+1;
  return partition;
}

bool next_partition(int n, vector<int>& partition) {
  if (partition.empty()) return false;
  auto it = partition.end();
  int max_value = n-1;
  while (--it >= partition.begin() && *it == max_value)
    --max_value;
  if (it < partition.begin()) {
    partition = new_partition(partition.size()+1);
    return false;
  }
  int s = *it;
  while (it != partition.end())
    *(it++) = ++s;
  return true;
}

typedef tuple<string,int,int> entry_t;
typedef map<entry_t,bool> cache_t;

bool is_valid(const map<string,rule_t>& rules, const string& input,
              const string& rule_id, int begin, int end, cache_t& cache) {
  if (rule_id[0] == '"')
    return input.substr(begin,end-begin) == rule_id.substr(1,rule_id.length()-2);
  auto key = make_tuple(rule_id,begin,end);
  auto entry_it = cache.find(key);
  if (entry_it == cache.end()) {
    bool ok = false;
    for (const auto& concat : rules.find(rule_id)->second) {
      if (concat.size() > end-begin) break;
      bool concat_ok = false;
      auto partition = new_partition(concat.size());
      do {
        bool part_ok = true;
        int begin_part = begin;
        for (int partid = 0; partid < partition.size() && part_ok; ++partid) {
          int end_part = begin+partition[partid];
          part_ok = is_valid(rules,input,concat[partid],begin_part,end_part,cache);
          begin_part = end_part;
        }
        part_ok = part_ok && is_valid(rules,input,concat.back(),begin_part,end,cache);
        concat_ok = part_ok;
      } while (next_partition(end-begin,partition) && !concat_ok);
      ok = concat_ok;
      if (ok) break;
    }
    entry_it = cache.emplace(key,ok).first;
  }
  return entry_it->second;
}

bool is_valid(const map<string,rule_t>& rules, const string& input) {
  cache_t cache;
  return is_valid(rules, input, "0", 0, input.size(), cache);
}

int main() {
  auto rules = parse_rules(cin);
  vector<string> inputs;
  string line;
  while (getline(cin,line) && !line.empty())
    inputs.push_back(move(line));
  int sum = 0;
  for (const auto& input : inputs) {
    bool valid = is_valid(rules,input);
    cout << input << ": " << boolalpha << valid << endl;
    sum += valid;
  }
  cout << sum << endl;
}

