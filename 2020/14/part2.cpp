#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

typedef vector<string> cmd_t;
typedef vector<cmd_t> program_t;
typedef unordered_map<string,string> memory_t;

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

void set(memory_t& memory, string& address, const string& value, unsigned i) {
  if (i == address.size())
    memory[address] = value;
  else {
    if (address[i] == 'X') {
      address[i] = '0';
      set(memory, address, value, i+1);
      address[i] = '1';
      set(memory, address, value, i+1);
      address[i] = 'X';
    }
    else
      set(memory, address, value, i+1);
  }
}

void set(memory_t& memory, string address, const string& value) {
  set(memory, address, value, 0);
}

string compose_address(uint64_t address, const string& mask) {
  string composed_address = mask;
  for (unsigned i = 0; i < mask.size(); ++i) {
    unsigned j = mask.size()-1-i;
    if (composed_address[j] == '0')
      composed_address[j] = ( (address>>i)&1 )? '1' : '0';
  }
  return composed_address;
}

uint64_t exec(const program_t& program) {
  string mask(36,'X');
  memory_t memory;
  for (const cmd_t& cmd : program) {
    if (cmd[0] == "mask")
      mask = cmd[1];
    else { // cmd[0] == "mem"
      string address = compose_address(stoull(cmd[1]), mask);
      set(memory,address,cmd[2]);
    }
  }
  uint64_t sum = 0;
  for (const auto&[address,value] : memory)
    sum += stoull(value);
  return sum;
}

int main() {
  program_t program;
  string line;
  while (getline(cin,line))
    program.push_back(split(line, " []="));
  cout << exec(program) << endl;
}

