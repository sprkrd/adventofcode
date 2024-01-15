#include <iostream>
#include <vector>
using namespace std;

int main() {
  vector<int> readings;
  int n;
  while (cin >> n) {
    readings.push_back(n);
  }
  int count = 0;
  for (unsigned i = 1; i < readings.size(); ++i) {
    count += readings[i] > readings[i-1];
  }
  cout << count << endl;
}
