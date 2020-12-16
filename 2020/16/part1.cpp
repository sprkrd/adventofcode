#include <algorithm>
#include <cassert>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
using namespace std;

typedef vector<int> ticket_t;

struct range_t {
  int min, max;
  bool contains(int x) const {
    return min <= x && x <= max;
  }
};

struct range_union_t {
  range_t range1, range2;
  bool contains(int x) const {
    return range1.contains(x) or range2.contains(x);
  }
};

struct rule_t {
  string field_name;
  range_union_t range;
};

vector<rule_t> parse_rules(istream& in) {
  static const regex re("([\\w\\s]+): (\\d+)-(\\d+) or (\\d+)-(\\d+)");
  vector<rule_t> rules;
  string line;
  while (getline(cin,line) && !line.empty()) {
    smatch match;
    assert(regex_match(line,match,re));
    rules.emplace_back();
    rules.back().field_name = match.str(1);
    rules.back().range.range1.min = stoi(match.str(2));
    rules.back().range.range1.max = stoi(match.str(3));
    rules.back().range.range2.min = stoi(match.str(4));
    rules.back().range.range2.max = stoi(match.str(5));
  }
  return rules;
}

vector<ticket_t> parse_tickets(istream& in) {
  static const regex re("(\\d+)(?:,|$)");
  vector<ticket_t> tickets;
  string line;
  getline(in,line);
  while (getline(in,line) && !line.empty()) {
    tickets.emplace_back();
    auto it = sregex_iterator(line.begin(), line.end(), re);
    auto end = sregex_iterator();
    for (; it != end; ++it)
      tickets.back().push_back(stoi(it->str(1)));
  }
  return tickets;
}

int error(const vector<rule_t>& rules, const ticket_t& ticket) {
  int sum = 0;
  for (int field_value : ticket) {
    bool valid_field = none_of(rules.begin(), rules.end(),
        [field_value](const rule_t& rule) {
        return rule.range.contains(field_value); });
    sum += valid_field*field_value;
  }
  return sum;
}

int main() {
  auto rules = parse_rules(cin);
  auto my_ticket = move(parse_tickets(cin)[0]);
  auto nearby_tickets = parse_tickets(cin);
  int err = 0;
  for (const auto& ticket : nearby_tickets)
    err += error(rules, ticket);
  cout << err << endl;
}

