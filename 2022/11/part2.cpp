#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <regex>
using namespace std;

typedef uint64_t integer;

integer k_old = numeric_limits<integer>::max();

struct Expression {
    enum Operation { add, mul };
    integer lhs, rhs;
    Operation operation;

    integer eval(integer old_value, integer m) {
        integer lhs_eval = lhs == k_old? old_value : lhs;
        integer rhs_eval = rhs == k_old? old_value : rhs;
        integer ans;
        switch (operation) {
            case add: ans = lhs_eval + rhs_eval; break;
            case mul: ans = lhs_eval * rhs_eval; break;
            default: assert(false);
        }
        ans %= m;
        return ans;
    }
};

ostream& operator<<(ostream& out, const Expression& exp) {
    if (exp.lhs == k_old) out << "old";
    else out << exp.lhs;
    out << ' ' << (exp.operation == Expression::add? '+' : '*') << ' ';
    if (exp.rhs == k_old) out << "old";
    else out << exp.rhs;
    return out;
}

vector<int> every_int(const string& str) {
    static const regex rex("[0-9]+,?");
    vector<int> ans;
    sregex_iterator it(str.begin(), str.end(), rex);
    for (; it != sregex_iterator(); ++it) {
        ans.push_back(stoi(it->str()));
    }
    return ans;
}

Expression parse_expression(const string& line) {
    static const regex rex("(old|[0-9]+) (\\+|\\*) (old|[0-9]+)");
    smatch m;
    assert(regex_search(line.begin(), line.end(), m, rex));
    Expression exp;
    exp.lhs = m[1] == "old"? k_old : stoll(m[1]);
    exp.operation = m[2] == "+"? Expression::add : Expression::mul;
    exp.rhs = m[3] == "old"? k_old : stoll(m[3]);
    return exp;
}

struct Monkey {
    int id;
    deque<integer> items;
    Expression operation;
    integer test;
    int if_true, if_false;
    integer inspected_items;

    bool read(istream& in) {
        items.clear();
        inspected_items = 0;
        string line;
        while (getline(in, line) && line.empty());
        if (line.empty()) return false;

        assert(line.substr(0, 6) == "Monkey");
        id = every_int(line)[0];

        assert(getline(in, line));
        for (int x : every_int(line))
            items.push_back(x);

        assert(getline(in, line));
        operation = parse_expression(line);

        assert(getline(in, line));
        test = every_int(line)[0];

        assert(getline(in, line));
        if_true = every_int(line)[0];

        assert(getline(in, line));
        if_false = every_int(line)[0];

        return true;
    }
};

ostream& operator<<(ostream& out, const Monkey& monkey) {
    out << "Monkey " << monkey.id << ":\n"
        << "  Starting items: ";
    for (unsigned i = 0; i < monkey.items.size(); ++i) {
        if (i > 0) out << ", ";
        out << monkey.items[i];
    }
    out << '\n'
        << "  Operation: new = " << monkey.operation << '\n'
        << "  Test: divisible by " << monkey.test << '\n'
        << "    If true: throw to monkey " << monkey.if_true << '\n'
        << "    If false: throw to monkey " << monkey.if_false << '\n';
    return out;
}

struct System {
    vector<Monkey> monkeys;
    integer m;

    void read(istream& in) {
        m = 1;
        monkeys.clear();
        Monkey monkey;
        while (monkey.read(in)) {
            monkeys.push_back(move(monkey));
            m *= monkey.test;
        }
    }

    void step(Monkey& monkey) {
        integer need_to_inspect = monkey.items.size();
        
        for (unsigned i = 0; i < need_to_inspect; ++i) {
            integer item_score = monkey.items.front();
            item_score = monkey.operation.eval(item_score, m);
            int next_monkey = item_score%monkey.test == 0? monkey.if_true : monkey.if_false;
            monkey.items.pop_front();
            monkeys[next_monkey].items.push_back(item_score);
        }

        monkey.inspected_items += need_to_inspect;
    }

    void step() {
        for (Monkey& monkey : monkeys) {
            step(monkey);
        }
    }

    integer monkey_business() const {
        vector<integer> inspections(monkeys.size());
        for (unsigned i = 0; i < monkeys.size(); ++i) {
            inspections[i] = monkeys[i].inspected_items;
        }
        sort(inspections.begin(), inspections.end(), greater<integer>());
        return inspections[0]*inspections[1]; 
    }
};

ostream& operator<<(ostream& out, const System& system) {
    for (const Monkey& monkey : system.monkeys) {
        out << monkey << endl;
    }
    return out;
}

int main() {
    System system;
    system.read(cin);
    int round = 0;
    //cout << "#######" << endl;
    //cout << "Round " << round << endl;
    //cout << "#######" << endl;
    //cout << system << endl;
    //cout << "---" << endl;
    for (int i = 0; i < 10000; ++i) {
        system.step();
        ++round;
        //cout << "#######" << endl;
        //cout << "Round " << round << endl;
        //cout << "#######" << endl;
        //cout << system << endl;
        //cout << "---" << endl;
    }
    cout << system.monkey_business() << endl;
}
