#include <iostream>
#include <set>
#include <string>
using namespace std;

void intersect_update(set<char>& l, const set<char>& r) {
  auto l_it = l.begin();
  auto r_it = r.begin();
  while (l_it != l.end() && r_it != r.end()) {
    if (*l_it < *r_it)
      l_it = l.erase(l_it);
    else if (*r_it < *l_it)
      ++r_it;
    else {
      ++l_it;
      ++r_it;
    }
  }
  l.erase(l_it, l.end());
}

int main() {
  string line;
  int count = 0;
  while (getline(cin,line)) {
    set<char> intersection(line.begin(),line.end());
    while (getline(cin,line) && !line.empty()) {
      set<char> questions(line.begin(), line.end());
      intersect_update(intersection, questions);
    }
    count += intersection.size();
  }
  cout << count << endl;
}

