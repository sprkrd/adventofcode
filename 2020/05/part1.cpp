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
  string seat_str;
  int max_id = -1;
  while (cin >> seat_str) {
    int id = seat_id(seat_str);
    if (id > max_id) max_id = id;
  }
  cout << max_id << endl;
}

