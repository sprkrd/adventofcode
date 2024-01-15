#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

#include <z3++.h>
using namespace std;

typedef int64_t integer;

struct Variable {
    string arg1;
    string arg2;
    char operation;
    integer value;
    bool atomic;
    bool evaluated;
};

ostream& operator<<(ostream& out, const Variable& variable) {
    if (!variable.atomic) {
        out << variable.arg1 << ' '
            << variable.operation << ' '
            << variable.arg2;
        if (variable.evaluated) {
            out << " (" << variable.value << ')';
        }
        return out;
    }
    if (variable.evaluated) {
        return out << variable.value;
    }
    return out << '?';
}

ostream& operator<<(ostream& out, const unordered_map<string,Variable>& tree) {
    for (auto&&[id,variable] : tree) {
        out << id << ": " << variable << '\n';
    }
    return out;
}

pair<string,Variable> parse(const string& line) {
    istringstream iss(line);
    Variable variable;
    string id;
    iss >> id;
    id = id.substr(0, id.length() - 1);

    string arg;
    iss >> arg;
    bool is_number = all_of(arg.begin(), arg.end(), [](char c){return isdigit(c);});
    if (is_number) {
        variable.value = stoll(arg);
        variable.evaluated = variable.atomic = true;
    }
    else {
        variable.arg1 = move(arg);
        iss >> variable.operation >> variable.arg2;
        variable.evaluated = variable.atomic = false;
    }
    if (id == "humn") {
        variable.evaluated = false;
        variable.atomic = true;
    }
    if (id == "root") {
        variable.operation = '=';
    }
    return make_pair(id, variable);
}

void solve(unordered_map<string,Variable>& tree) {
    z3::context ctx;
    z3::solver solver(ctx);
    for (auto&&[id, variable] : tree) {
        auto z3_var = ctx.int_const(id.c_str());
        if (variable.atomic && variable.evaluated) {
            solver.add(z3_var == ctx.int_val(variable.value));
        }
        else if (!variable.atomic) {
            auto z3_arg1 = ctx.int_const(variable.arg1.c_str());
            auto z3_arg2 = ctx.int_const(variable.arg2.c_str());
            switch (variable.operation) {
                case '+':
                    solver.add(z3_var == z3_arg1 + z3_arg2);
                    break;
                case '-':
                    solver.add(z3_var == z3_arg1 - z3_arg2);
                    break;
                case '*':
                    solver.add(z3_var == z3_arg1 * z3_arg2);
                    break;
                case '/':
                    solver.add(z3_var == z3_arg1 / z3_arg2);
                    break;
                case '=':
                    solver.add(z3_var == z3::ite(z3_arg1 == z3_arg2, ctx.int_val(1), ctx.int_val(0)));
                    break;
                default:
                    assert(false);
            }
        }
    }
    auto z3_root = ctx.int_const("root");
    solver.add(z3_root == 1);
    assert(solver.check() == z3::sat);
    auto model = solver.get_model();
    for (unsigned i = 0; i < model.num_consts(); ++i) {
        auto fun = model[i];
        auto value = model.get_const_interp(fun).get_numeral_int64();
        auto& variable = tree.at(fun.name().str());
        variable.value = value;
        variable.evaluated = true;
    }
}


int main() {
    unordered_map<string,Variable> tree;
    string line;
    while (getline(cin,line)) {
        tree.insert(parse(line));
    }
    
    solve(tree);
    cout << tree << endl;
    cout << "---" << endl;
    cout << "humn: " << tree.at("humn") << endl;
}
