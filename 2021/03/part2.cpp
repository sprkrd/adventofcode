#include <cassert>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

char most_common_bit(const vector<string>& list, int pos) {
  int n_ones = 0;
  for (const auto& num : list) {
    n_ones += num[pos] == '1';
  }
  return 2*n_ones >= list.size()? '1' : '0';
}

vector<string> filter(const vector<string>& list, int pos, char bit) {
  vector<string> filtered;
  filtered.reserve(list.size());
  for (const auto& num : list) {
    if (num[pos] == bit) {
      filtered.push_back(num);
    }
  }
  return filtered;
}

int main() {
  
  vector<string> list;
  string num;
  while (cin >> num) {
    list.push_back(num);
  }
  
  string oxygen;
  {
    auto candidates = list;
    int pos = 0;
    while (candidates.size() > 1) {
      char mcb = most_common_bit(candidates, pos);
      candidates = filter(candidates, pos, mcb);
      pos += 1;
    }
    assert(candidates.size() == 1);
    oxygen = candidates.back();
  }
  
  string co2;
  {
    auto candidates = list;
    int pos = 0;
    while (candidates.size() > 1) {
      char lcb = most_common_bit(candidates, pos) == '0'? '1' : '0';
      candidates = filter(candidates, pos, lcb);
      pos += 1;
    }
    assert(candidates.size() == 1);
    co2 = candidates.back();
  }
  
  int oxygen_int = stoi(oxygen, nullptr, 2);
  int co2_int = stoi(co2, nullptr, 2);
  int ans = oxygen_int*co2_int;
  cout << ans << endl;
}
