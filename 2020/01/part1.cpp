#include <iostream>
#include <set>
using namespace std;

int main() {
  set<int> seen;
  int entry;
  while (cin >> entry) {
    int remaining = 2020 - entry;
    if (seen.count(remaining)) {
      cout << entry*remaining << endl;
      return 0;
    }
    seen.insert(entry);
  }
  cerr << "Not found! I've must messed up :(\n";
  return 1;
}
