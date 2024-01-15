#include <iostream>
#include <utility>
#include <map>
#include <vector>
using namespace std;

enum class op_t { nop, acc, jmp };
typedef pair<op_t,int> instruction_t;
typedef vector<instruction_t> program_t;

int exec(const program_t& program) {
  vector<bool> already_executed(program.size());
  int acc = 0;
  int pc = 0;
  while (!already_executed[pc]) {
    already_executed[pc] = true;
    auto[op,arg] = program[pc];
    switch (op) {
      case op_t::acc:
        acc += arg;
      case op_t::nop:
        pc += 1;
        break;
      case op_t::jmp:
        pc += arg;
    }
  }
  return acc;
}

program_t read_program(istream& in) {
  static const map<string,op_t> op_table {
    {"nop",op_t::nop}, {"acc",op_t::acc}, {"jmp",op_t::jmp}};
  program_t program;
  string opstr;
  int arg;
  while (in >> opstr >> arg) {
    op_t op = op_table.find(opstr)->second;
    program.emplace_back(op,arg);
  }
  return program;
}

int main() {
  auto program = read_program(cin);
  cout << exec(program) << endl;
}

