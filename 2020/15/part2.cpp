#include <iostream>
#include <vector>
using namespace std;

// reminiscent of the van eck sequence: https://oeis.org/A181391

const unsigned i_requested = 30000000;

int main() {
  vector<int> most_recent_appearance(i_requested,-1);
  unsigned i = 0;
  unsigned a_i;
  while (cin >> a_i)
    most_recent_appearance[a_i] = i++;

  while (i < i_requested) {
    int last_appearance = most_recent_appearance[a_i];
    most_recent_appearance[a_i] = i-1;
    a_i = last_appearance==-1? 0 : i - last_appearance - 1;
    ++i;
  }

  cout << a_i << endl;
}

