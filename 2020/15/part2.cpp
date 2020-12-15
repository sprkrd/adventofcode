#include <iostream>
#include <vector>
using namespace std;

// reminiscent of the van eck sequence: https://oeis.org/A181391

int main() {
  vector<int> most_recent_appearance;
  unsigned i = 0;
  unsigned a_i;
  while (cin >> a_i) {
    if (most_recent_appearance.size() <= a_i)
      most_recent_appearance.resize(a_i+1,-1);
    most_recent_appearance[a_i] = i++;
  }

  while (i < 30000000) {
    if (most_recent_appearance.size() <= a_i)
      most_recent_appearance.resize(a_i+1,-1);
    int last_appearance = most_recent_appearance[a_i];
    most_recent_appearance[a_i] = i-1;
    a_i = last_appearance==-1? 0 : i - last_appearance - 1;
    ++i;
  }

  cout << a_i << endl;
}

