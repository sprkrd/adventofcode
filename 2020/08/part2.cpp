#include <cassert>
#include <iostream>
#include <map>
#include <utility>
#include <vector>
using namespace std;

enum class op_t { nop, acc, jmp };
typedef pair<op_t,int> instruction_t;
typedef vector<instruction_t> program_t;

struct status_t {
  vector<bool> already_executed;
  int pc, acc, allowed_changes;
};

bool exec(const program_t& program, status_t& status) {
  int pc = status.pc;
  int acc = status.acc;
  int allowed_changes = status.allowed_changes;
  if (allowed_changes < 0)
    return false;
  if ((unsigned)pc >= program.size())
    return true;
  if (status.already_executed[pc])
    return false;
  status.already_executed[pc] = true;
  auto[op,arg] = program[pc];
  if (op == op_t::acc) {
    status.acc += arg;
    status.pc += 1;
    if (exec(program,status)) return true;
  }
  else {
    // try nop
    status.allowed_changes = allowed_changes - (op!=op_t::nop);
    status.pc = pc + 1;
    if (exec(program,status)) return true;
    // try jmp
    status.allowed_changes = allowed_changes - (op!=op_t::jmp);
    status.pc = pc + arg;
    if (exec(program,status)) return true;
  }
  // restore status
  status.pc = pc;
  status.acc = acc;
  status.allowed_changes = allowed_changes;
  status.already_executed[pc] = false;
  return false;
}

int exec(const program_t& program) {
  status_t status = {vector<bool>(program.size()), 0, 0, 1};
  assert(exec(program,status));
  return status.acc;
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

