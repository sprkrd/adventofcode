#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
using namespace std;

typedef int64_t integer;

struct Variable {
    string arg1;
    string arg2;
    char operation;
    integer value;
    bool evaluated;
};

ostream& operator<<(ostream& out, const Variable& variable) {
    if (variable.evaluated) {
        return out << variable.value;
    }
    return out << variable.arg1 << ' '
               << variable.operation << ' '
               << variable.arg2;
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
        variable.evaluated = true;
    }
    else {
        variable.arg1 = move(arg);
        iss >> variable.operation >> variable.arg2;
        variable.evaluated = false;
    }
    return make_pair(id, variable);
}

integer eval(unordered_map<string,Variable>& tree, const string& id) {
    Variable& var = tree.at(id);
    if (var.evaluated) {
        return var.value;
    }
    integer arg1 = eval(tree, var.arg1);
    integer arg2 = eval(tree, var.arg2);
    switch (var.operation) {
        case '+':
            var.value = arg1 + arg2;
            break;
        case '-':
            var.value = arg1 - arg2;
            break;
        case '*':
            var.value = arg1 * arg2;
            break;
        case '/':
            assert(arg2 != 0 && arg1%arg2 == 0);
            var.value = arg1 / arg2;
            break;
        default:
            assert(false);
            break;
    }
    var.evaluated = true;
    return var.value;
}

int main() {
    unordered_map<string,Variable> tree;
    string line;
    while (getline(cin,line)) {
        tree.insert(parse(line));
    }
    cout << eval(tree, "root") << endl;
}
