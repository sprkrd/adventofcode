#include <cstdint>
#include <iostream>
using namespace std;

const uint64_t mult = 7;
const uint64_t mod = 20201227;

uint64_t discrete_logarithm(uint64_t target, uint64_t base, uint64_t mod) {
  uint64_t value = 1;
  uint64_t exp = 0;
  while (value != target) {
    value = (value*base)%mod;
    ++exp;
  }
  return exp;
}

uint64_t discrete_pow(uint64_t base, uint64_t exp, uint64_t mod) {
  if (exp == 0) return 1;
  uint64_t base_pow_exp_2 = discrete_pow(base,exp/2,mod);
  return ( base_pow_exp_2*base_pow_exp_2*(exp&1?base:1) )%mod;
}

int main() {
  uint64_t A, B;
  cin >> A >> B;
  uint64_t a = discrete_logarithm(A, mult, mod);
  uint64_t b = discrete_logarithm(B, mult, mod);
  uint64_t key = discrete_pow(mult, a*b, mod);
  cout << key << endl;
}

