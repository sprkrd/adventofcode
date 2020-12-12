#include <iostream>
#include <set>
#include <string>
using namespace std;

int main() {
  string line;
  int count = 0;
  while (getline(cin,line)) {
    set<char> common_questions;
    while (!line.empty()) {
      for (char q : line)
        common_questions.insert(q);
      getline(cin,line);
    }
    count += common_questions.size();
  }
  cout << count << endl;
}

