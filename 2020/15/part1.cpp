#include <iostream>
#include <vector>
using namespace std;

// reminiscent of the van eck sequence: https://oeis.org/A181391

int main() {
  vector<int> last_occurrence;
  unsigned i = 0;
  unsigned last,second_to_last;
  while (cin >> last) {
    if (i > 0) {
      if (last_occurrence.size() <= second_to_last)
        last_occurrence.resize(second_to_last+1,-1);
      last_occurrence[second_to_last] = i-1;
    }
    second_to_last = last;
    ++i;
  }

  while (i < 2020) {
    second_to_last = last;
    if (last_occurrence.size() <= second_to_last)
      last_occurrence.resize(second_to_last+1,-1);
    if (last_occurrence[second_to_last] == -1)
      last = 0;
    else
      last = i - last_occurrence[second_to_last] - 1;
    last_occurrence[second_to_last] = i-1;
    ++i;
  }

  cout << last << endl;
}

