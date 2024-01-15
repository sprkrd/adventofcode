#include <iostream>
#include <set>
using namespace std;

int main() {
  set<int> seen;
  int entry;
  while (cin >> entry)
    seen.insert(entry);

  auto it1 = seen.begin();
  while (it1 != seen.end()) {
    auto it2 = it1;
    while (++it2 != seen.end()) {
      int remaining = 2020 - (*it1 + *it2);
      if (seen.count(remaining)) {
        int product = *it1 * *it2 * remaining;
        cout << product << endl;
        return 0;
      }
    }
    ++it1;
  }

  cerr << "Not found! I've must messed up :(\n";
  return 1;
}
