#include <cstdint>
#include <iostream>
#include <vector>
using namespace std;

const int64_t k_target = 29221323;

int main(int argc, char* argv[]) {
  vector<int64_t> numbers;
  int64_t x;
  while (cin >> x)
    numbers.push_back(x);
  unsigned lo = 0, hi = 1;
  int64_t sum = numbers[lo] + numbers[hi];
  while (true) {
    if (sum < k_target)
      sum += numbers[++hi];
    else if (sum > k_target)
      sum -= numbers[lo++];
    else
      break;
  }
  int64_t min = numbers[lo];
  int64_t max = numbers[lo];
  for (unsigned i = lo+1; i <= hi; ++i) {
    if (numbers[i] < min) min = numbers[i];
    if (numbers[i] > max) max = numbers[i];
  }
  cout << (min+max) << endl;
}

