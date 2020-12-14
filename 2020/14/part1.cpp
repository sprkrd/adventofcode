#include <algorithm>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

typedef vector<string> cmd_t;
typedef vector<cmd_t> program_t;
typedef bitset<36> value_t;

vector<string> split(const string& str, const string& delim=" \n\t") {
  vector<string> parts;
  size_t pos = 0;
  while (pos < str.length()) {
    size_t pos_next = str.find_first_of(delim, pos);
    if (pos_next > pos)
      parts.push_back(str.substr(pos, pos_next-pos));
    pos = pos_next == string::npos? string::npos : pos_next + 1;
  }
  return parts;
}

uint64_t exec(const program_t& program) {
  unordered_map<value_t,value_t> memory;
  value_t mask_1s = 0;
  value_t mask_0s = 0;
  for (const cmd_t& cmd : program) {
    if (cmd[0] == "mask") {
      auto mask_1s_str = cmd[1];
      auto mask_0s_str = cmd[1];
      replace_if(mask_1s_str.begin(), mask_1s_str.end(), [](char c) { return c != '1'; }, '0');
      replace_if(mask_0s_str.begin(), mask_0s_str.end(), [](char c) { return c != '0'; }, '1');
      mask_1s = value_t(mask_1s_str);
      mask_0s = value_t(mask_0s_str);
    }
    else { // cmd[0] == mem
      value_t address(stoull(cmd[1]));
      auto value = (value_t(stoull(cmd[2]))|mask_1s)&mask_0s;
      memory[address] = value;
    }
  }
  uint64_t sum = 0;
  for (auto[address,value] : memory)
    sum += value.to_ullong();
  return sum;
}

int main() {
  program_t program;
  string line;
  while (getline(cin,line))
    program.push_back(split(line, " []="));
  cout << exec(program) << endl;
}

