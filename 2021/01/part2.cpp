#include <iostream>
#include <vector>
using namespace std;

int main() {
  vector<int> readings;
  int n;
  while (cin >> n) {
    readings.push_back(n);
  }

  vector<int> filtered_readings(readings.size()-2);

  for (unsigned i = 1; i < readings.size() - 1; ++i) {
    filtered_readings[i-1] = readings[i-1] + readings[i] + readings[i+1];
  }


  int count = 0;
  for (unsigned i = 1; i < filtered_readings.size(); ++i) {
    count += filtered_readings[i] > filtered_readings[i-1];
  }
  cout << count << endl;
}
