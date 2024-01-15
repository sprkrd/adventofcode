#include <iostream>
#include <set>
#include <string>
using namespace std;

int main() {
  string line;
  int count = 0;
  while (getline(cin,line)) {
    set<char> q_union;
    while (!line.empty()) {
      q_union.insert(line.begin(),line.end());
      getline(cin,line);
    }
    count += q_union.size();
  }
  cout << count << endl;
}

