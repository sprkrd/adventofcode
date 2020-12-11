#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct policy_t {
  int i, j;
  char character;
};

struct entry_t {
  policy_t policy;
  string password;
};

istream& operator>>(istream& in, entry_t& entry) {
  string line;
  if (getline(in, line)) {
    size_t pos_dash = line.find('-');
    size_t pos_first_space = line.find(' ', pos_dash+1);
    size_t pos_colon = line.find(':', pos_first_space+1);
    entry.policy.i = stoi(line.substr(0, pos_dash));
    entry.policy.j = stoi(line.substr(pos_dash+1, pos_first_space-pos_dash-1));
    entry.policy.character = line[pos_first_space+1];
    entry.password = line.substr(pos_colon+2);
  }
  return in;
}

ostream& operator<<(ostream& out, const entry_t& entry) {
  return out << entry.policy.i << '-' << entry.policy.j
             << ' ' << entry.policy.character << ": " << entry.password;
}

bool is_valid_entry(const entry_t& entry) {
  int i = entry.policy.i-1;
  int j = entry.policy.j-1;
  char c = entry.policy.character;
  return (entry.password[i]==c)^(entry.password[j]==c);
}

int main() {
  int number_of_valid_entries = 0;
  entry_t entry;
  while (cin >> entry) {
    if (is_valid_entry(entry))
      ++number_of_valid_entries;
  }
  cout << number_of_valid_entries << endl;
}
