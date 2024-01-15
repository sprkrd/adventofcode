#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
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

vector<int> parse_bus_ids(const string& line) {
  auto parts = split(line, ",");
  vector<int> result(parts.size());
  for (unsigned i = 0; i < parts.size(); ++i)
    result[i] = parts[i]=="x"? -1 : stoi(parts[i]);
  return result;
}

uint64_t negmod(uint64_t x, uint64_t y) {
  uint64_t result = y - x%y;
  return result == y? 0 : result;
}

uint64_t find_t (const vector<int>& bus_ids) {
  uint64_t cumprod = 1;
  uint64_t t = 0;
  for (unsigned i = 0; i < bus_ids.size(); ++i) {
    if (bus_ids[i] == -1) continue;
    while (negmod(t+i,bus_ids[i])) 
      t += cumprod;
    cumprod *= bus_ids[i];
  }
  return t;
}

int main() {
  int earliest_time;
  string line;
  cin >> earliest_time;
  cin.ignore(numeric_limits<streamsize>::max(),'\n');
  getline(cin,line);
  auto bus_ids = parse_bus_ids(line);
  auto t = find_t(bus_ids);
  cout << t << endl;
}

