#include <algorithm>
#include <iostream>
#include <string>
using namespace std;

int seat_id(const string& seat_str) {
  int id = 0;
  for (char c : seat_str)
    id = id*2 + (c=='B'||c=='R');
  return id;
}

int main() {
  vector<int> ids;
  string seat_str;
  while (cin >> seat_str) {
    int id = seat_id(seat_str);
    ids.push_back(id);
  }
  sort(ids.begin(), ids.end());
  for (unsigned i = 1; i < ids.size(); ++i) {
    if (ids[i] == ids[i-1] + 2)
      cout << (ids[i]-1) << endl;
  }
}

