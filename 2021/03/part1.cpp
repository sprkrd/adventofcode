#include <iostream>
#include <string>
#include <vector>
using namespace std;



int main() {
  
  vector<string> list;
  string num;
  while (cin >> num) {
    list.push_back(num);
  }
  
  int nbits = list[0].length();
  
  string gamma_rate(nbits, '0');
  string epsilon_rate(nbits, '1');
  for (int i = 0; i < nbits; ++i) {
    int n_ones = 0;
    for (const auto& n : list) {
      n_ones += n[i] == '1';
    }
    if (n_ones > list.size()/2) {
      gamma_rate[i] = '1';
      epsilon_rate[i] = '0';
    }
  }
  
  int gamma_rate_int = stoi(gamma_rate, nullptr, 2);
  int epsilon_rate_int = stoi(epsilon_rate, nullptr, 2);
  int ans = gamma_rate_int*epsilon_rate_int;
  cout << ans << endl;
  
}
