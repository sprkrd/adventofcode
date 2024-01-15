#include <algorithm>
#include <cassert>
#include <iostream>
#include <regex>
#include <set>
#include <string>
#include <vector>
using namespace std;

typedef vector<int> ticket_t;

ostream& operator<<(ostream& out, const ticket_t& ticket) {
  for (unsigned i = 0; i < ticket.size(); ++i) {
    if (i > 0) out << ',';
    out << ticket[i];
  }
  return out;
}

struct range_t {
  int min, max;
  bool contains(int x) const {
    return min <= x && x <= max;
  }
};

ostream& operator<<(ostream& out, const range_t& range) {
  return out << range.min << '-' << range.max;
}

struct range_union_t {
  range_t range1, range2;
  bool contains(int x) const {
    return range1.contains(x) or range2.contains(x);
  }
};

ostream& operator<<(ostream& out, const range_union_t& range) {
  return out << range.range1 << " or " << range.range2;
}

struct rule_t {
  string field_name;
  range_union_t range;
};

ostream& operator<<(ostream& out, const rule_t& rule) {
  return out << rule.field_name << ": " << rule.range;
}

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

bool valid(const vector<rule_t>& rules, const ticket_t& ticket) {
  for (int field_value : ticket) {
    if (none_of(rules.begin(), rules.end(),
        [field_value](const rule_t& rule) {
        return rule.range.contains(field_value); }))
      return false;
  }
  return true;
}

void filter_tickets(const vector<rule_t>& rules, vector<ticket_t>& tickets) {
  vector<ticket_t> filtered;
  filtered.reserve(tickets.size());
  for (const auto& ticket : tickets) {
    if (valid(rules, ticket))
      filtered.push_back(ticket);
  }
  tickets.swap(filtered);
}

vector<int> plausible_rules(const vector<rule_t>& rules, const vector<ticket_t>& tickets, int field) {
  vector<int> plausible;
  for (unsigned i = 0; i < rules.size(); ++i) {
    const auto& rule = rules[i];
    if (all_of(tickets.begin(), tickets.end(), [field,&rule](const ticket_t& ticket) {
        return rule.range.contains(ticket[field]); }))
      plausible.push_back(i);
  }
  return plausible;
}

struct context_t {
  vector<int> variables;
  vector<int> solution;
  set<int> available;
  unsigned i;
};

bool csp(const vector<vector<int>>& domain, context_t& context) {
  if (context.i >= domain.size())
    return true;
  unsigned i = context.i++;
  int var = context.variables[i];
  for (int value : domain[var]) {
    if (!context.available.count(value)) continue;
    context.available.erase(value);
    context.solution[var] = value;
    if (csp(domain, context))
      return true;
    context.available.insert(value);
  }
  context.i = i;
  return false;
}

vector<int> csp(const vector<vector<int>>& domain) {
  context_t context;
  context.variables.resize(domain.size());
  for (unsigned i = 0; i < domain.size(); ++i) {
    context.variables[i] = i;
    context.available.insert(i);
  }
  sort(context.variables.begin(), context.variables.end(), [&domain](int l, int r) {
      return domain[l].size() < domain[r].size(); });
  context.solution.resize(domain.size());
  context.i = 0;
  assert(csp(domain,context));
  return context.solution;
}

int main() {
  auto rules = parse_rules(cin);
  auto my_ticket = move(parse_tickets(cin)[0]);
  auto nearby_tickets = parse_tickets(cin);
  filter_tickets(rules,nearby_tickets);
  auto all_tickets = nearby_tickets;
  all_tickets.push_back(my_ticket);
  filter_tickets(rules, all_tickets);
  vector<vector<int>> domain(my_ticket.size());
  for (unsigned field = 0; field < my_ticket.size(); ++field)
    domain[field] = plausible_rules(rules, all_tickets, field);
  auto solution = csp(domain);
  cout << solution << endl;
  unsigned long long product = 1;
  for (unsigned i = 0; i < my_ticket.size(); ++i) {
    int rule_id = solution[i];
    if (rules[rule_id].field_name.substr(0,9) == "departure")
      product *= my_ticket[i];
  }
  cout << product << endl;
}

