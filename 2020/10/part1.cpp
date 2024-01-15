#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int main() {
  vector<int> joltages;
  int jolts;
  while (cin >> jolts)
    joltages.push_back(jolts);
  sort(joltages.begin(), joltages.end());
  int diffs_of_1 = joltages[0] == 1;
  int diffs_of_3 = 1 + (joltages[0]==3);
  for (unsigned i = 1; i <= joltages.size(); ++i) {
    int diff = joltages[i] - joltages[i-1];
    diffs_of_1 += diff == 1;
    diffs_of_3 += diff == 3;
  }
  int product = diffs_of_1*diffs_of_3;
  cout << product << endl;
}

