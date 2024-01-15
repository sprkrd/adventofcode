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

int main() {
  int earliest_time;
  string line;
  cin >> earliest_time;
  cin.ignore(numeric_limits<streamsize>::max(),'\n');
  getline(cin,line);
  auto bus_ids = parse_bus_ids(line);
  int selected_id = -1;
  int min_waiting_time = numeric_limits<int>::max();
  for (int id : bus_ids) {
    if (id != -1) {
      int waiting_time = (id-earliest_time%id)%id;
      if (waiting_time < min_waiting_time) {
        selected_id = id;
        min_waiting_time = waiting_time;
      }
    }
  }
  int product = selected_id * min_waiting_time;
  cout << selected_id << '*' << min_waiting_time
       << '=' << product << endl;
}

