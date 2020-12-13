#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>
using namespace std;

int main() {
  vector<int> joltages{0};
  int jolts;
  while (cin >> jolts)
    joltages.push_back(jolts);
  sort(joltages.begin(), joltages.end());
  joltages.push_back(joltages.back()+3);
  vector<uint64_t> num_of_arrangements(joltages.size());
  num_of_arrangements.back() = 1;
  for (int i = num_of_arrangements.size()-2; i >= 0; --i) {
    unsigned j = i+1;
    while (j < num_of_arrangements.size() && joltages[j] <= joltages[i]+3)
      num_of_arrangements[i] += num_of_arrangements[j++];
  }
  cout << num_of_arrangements[0] << endl;
}

