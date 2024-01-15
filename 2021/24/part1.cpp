#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

const char* register_names = "wxyz";

struct Argument {
    enum ArgumentType {lit, reg};
    ArgumentType argument_type;
    int value;
    bool operator==(const Argument& other) const {
        return argument_type == other.argument_type && value == other.value;
    }
};

ostream& operator<<(ostream& out, const Argument& arg) {
    switch (arg.argument_type) {
        case Argument::lit: return out << arg.value;
        case Argument::reg: return out << register_names[arg.value];
    }
    return out;
}

struct Operation {
    enum Verb {inp, add, mul, div, mod, eql};
    Verb verb;
    Argument arg0, arg1;
    bool operator==(const Operation& other) const {
        return verb == other.verb && arg0 == other.arg0 && (verb == inp || arg1 == other.arg1);
    }
    bool operator!=(const Operation& other) const {
        return !(*this == other);
    }
};

ostream& operator<<(ostream& out, Operation::Verb verb) {
    switch (verb) {
        case Operation::inp: out << "inp"; break;
        case Operation::add: out << "add"; break;
        case Operation::mul: out << "mul"; break;
        case Operation::div: out << "div"; break;
        case Operation::mod: out << "mod"; break;
        case Operation::eql: out << "eql"; break;
    }
    return out;
}

ostream& operator<<(ostream& out, const Operation& op) {
    out << op.verb << ' ' << op.arg0;
    if (op.verb != Operation::inp)
        out << ' ' <<  op.arg1;
    return out;
}

Operation::Verb verb_from_str(const string& verb) {
    if (verb == "inp")
        return Operation::inp;
    else if (verb == "add")
        return Operation::add;
    else if (verb == "mul")
        return Operation::mul;
    else if (verb == "div")
        return Operation::div;
    else if (verb == "mod")
        return Operation::mod;
    else if (verb == "eql")
        return Operation::eql;
    assert(false);
}

struct Program {
    vector<Operation> operations;

    void read(istream& in) {
        string line;
        while (getline(in, line)) {
            istringstream line_stream(line);
            Operation op;
            string verb, arg0;
            line_stream >> verb >> arg0;
            op.verb = verb_from_str(verb);
            op.arg0.argument_type = Argument::reg;
            op.arg0.value = arg0[0]-'w';
            if (verb != "inp") {
                string arg1;
                line_stream >> arg1;
                if (isdigit(arg1.back())) {
                    op.arg1.argument_type = Argument::lit;
                    op.arg1.value = stoi(arg1);
                }
                else {
                    op.arg1.argument_type = Argument::reg;
                    op.arg1.value = arg1[0]-'w';
                }
            }
            operations.push_back(op);
        }
    }
};

ostream& operator<<(ostream& out, const Program& program) {
    for (const Operation& op : program.operations)
        out << op << '\n';
    return out;
}

class Context {
    public:

        void init(uint64_t input = 0) {
            p_registers.fill(0);
            while (input) {
                p_input_stack.push_back(input%10);
                input /= 10;
            }
        }

        void exec(const Operation& op) {
            auto& dst = p_registers[op.arg0.value];
            switch (op.verb) {
                case Operation::inp: {
                    dst = next_input();
                    break;
                }
                case Operation::add: {
                    dst += get_value(op.arg1);
                    break;
                }
                case Operation::mul: {
                    dst *= get_value(op.arg1);
                    break;
                }
                case Operation::div: {
                    int arg1 = get_value(op.arg1);
                    assert(arg1 != 0);
                    dst /= arg1;
                    break;
                }
                case Operation::mod: {
                    int arg1 = get_value(op.arg1);
                    assert(arg1 != 0);
                    dst %= arg1;
                    break;
                }
                case Operation::eql: {
                    dst = dst == get_value(op.arg1);
                    break;
                }
            }
        }

        void run(const Program& program, uint64_t input = 0) {
            init(input);
            for (const Operation& op : program.operations) {
                exec(op);
            }
        }

        int w() const {
            return p_registers[0];
        }

        int x() const {
            return p_registers[1];
        }

        int y() const {
            return p_registers[2];
        }

        int z() const {
            return p_registers[3];
        }

    private:

        int get_value(const Argument& arg) const {
            switch (arg.argument_type) {
                case Argument::lit: return arg.value;
                case Argument::reg: return p_registers[arg.value];
            }
            assert(false);
        }

        int next_input() {
            int ret = 0;
            if (!p_input_stack.empty()) {
                ret = p_input_stack.back();
                p_input_stack.pop_back();
            }
            return ret;
        }

        array<int64_t,4> p_registers;
        vector<int> p_input_stack;

        friend ostream& operator<<(ostream& out, const Context& ctx) {
            out << "w = " << ctx.w() << '\n'
                << "x = " << ctx.x() << '\n'
                << "y = " << ctx.y() << '\n'
                << "z = " << ctx.z() << '\n';
            out << "input stack:";
            for (int i : ctx.p_input_stack)
                out << ' ' << i;
            return out;
        }
        
};

constexpr int A[] = {1, 1, 1, 26, 1, 26, 26, 1, 1, 1, 26, 26, 26, 26};
constexpr int B[] = {10, 14, 14, -13, 10, -13, -7, 11, 10, 13, -4, -9, -13, -9};
constexpr int C[] = {2, 13, 13, 9, 15, 3, 6, 5, 16, 1, 6, 3, 7, 9};

int next_z(int a, int b, int c, int input, int previous_z) {
    int x = (previous_z%26 + b) != input;
    int z = (previous_z/a)*(25*x+1) + (input+c)*x;
    return z;
}

vector<pair<int,int>> trace_back(int a, int b, int c, int z) {
    vector<pair<int,int>> candidates;
    // x = (previous_z%26 + b) != input;
    // z = (previous_z/a)*(25*x+1) + (input+c)*x;
    // z - (input+c)*x = (previous_z/a)*(25*x + 1);
    // x = 1
    //      check (z - (input+c)) % 26 = 0
    //      (z - (input+c))/26 + ?/a = (previous_z/a), ? < a
    //      a*(z - (input+c))/26 + ? = previous_z
    //      max(0, a*(z-(input+c))/26) <= previous_z <= a*(z-(input+c))/26 + (a-1)
    //      check (previous_z%26 + b) != input
    // x = 0
    //      z + ?/a = previous_z/a, ? < a
    //      a*z + ? = previous_z
    //      a*z <= previous_z <= a*z + (a-1)
    //      check (previous_z%26 + b) == input
    for (int input = 1; input <= 9; ++input) {
        {
            // assumption #1, x = 1
            int min_previous_z = max(0, a*(z-(input+c))/26);
            int max_previous_z = a*(z-(input+c))/26 + a-1;
            for (int previous_z = min_previous_z; previous_z <= max_previous_z; ++previous_z) {
                int x = (previous_z%26 + b) != input;
                if ((z - (input+c))%26 != 0) continue;
                if (x != 1) continue;
                candidates.emplace_back(previous_z, input);
                int z_ = next_z(a, b, c, input, previous_z);
                if (z_ != z) {
                    assert(false);
                }
            }
        }
        {
            // assumption #2, x = 0
            int min_previous_z = a*z;
            int max_previous_z = a*z + a-1;
            for (int previous_z = min_previous_z; previous_z <= max_previous_z; ++previous_z) {
                int x = (previous_z%26 + b) != input;
                if (x == 1) continue;
                candidates.emplace_back(previous_z, input);
                int z_ = next_z(a, b, c, input, previous_z);
                if (z_ != z) {
                    cout << "B >>> " << z_ << ' ' << z << ' ' << previous_z << ' ' << input << endl;
                    assert(false);
                }
            }
        }
    }
    return candidates;
}

int64_t model() {
    typedef pair<int,int> Point2i;
    map<Point2i,int64_t> cache;
    function<int64_t(int,int)> _model = [&](int z, int i) {
        auto [it,inserted] = cache.insert({Point2i(z,i), 0}); 
        if (inserted) {
            int64_t result = -1;
            if (i > 0) {
                auto candidates = trace_back(A[i-1], B[i-1], C[i-1], z);
                for (auto[prev_z,input] : candidates) {
                    int64_t m = _model(prev_z, i-1);
                    if (m >= 0) {
                        m = m*10 + input;
                    }
                    result = max(result, m);
                }
            }
            else if (z == 0) {
                result = 0;
            }
            it->second = result;
        }
        return it->second;
    };
    return _model(0, 14);
}

int main(int arg, char* argv[]) {
    /*
    Program qqo ,rogram;
    program.read(cin);
    Context ctx;
    uint64_t input = stoull(argv[1]);
    ctx.run(program, input);
    cout << ctx.z() << endl;
    */

    uint64_t input = model();
    cout << input << endl;

    vector<int> I;
    while (input) {
        I.push_back(input%10);
        input /= 10;
    }
    reverse(I.begin(), I.end());

    int z = 0;
    for (int i = 0; i < 14; ++i) {
        z = next_z(A[i], B[i], C[i], I[i], z);
    }
    cout << z << endl;    
}
