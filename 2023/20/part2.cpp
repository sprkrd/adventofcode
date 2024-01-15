#include <bitset>
#include <cassert>
#include <iostream>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
using namespace std;

#define DEBUG 0

class Module {

    public:

        enum Type { broadcaster, flipflop, conjunction };

        Module(string label, Type type = broadcaster) : label(move(label)),
                                                        type(type),
                                                        latched_value(0) {
        }

        Type get_type() const {
            return type;
        }

        void set_type(Type type) {
            this->type = type;
        }

        void connect(Module* output) {
            outputs.push_back(output);
            output->inputs.push_back(this);
        }

        pair<int,bool> ping(int pulse) {
            bool echo = false;
            switch (type) {
                case broadcaster:
                    latched_value = pulse;
                    echo = true;
                    break;
                case flipflop:
                    if (pulse == 0) {
                        latched_value = 1 - latched_value;
                        echo = true;
                    }
                    break;
                case conjunction:
                    latched_value = 0;
                    for (Module* in : inputs) {
                        if (in->latched_value == 0) {
                            latched_value = 1;
                            break;
                        }
                    }
                    echo = true;
                    break;
                default:
                    break;
            }
            return {latched_value, echo};
        }

        size_t number_of_outputs() const {
            return outputs.size();
        }

        Module* get_output(size_t i) const {
            return outputs[i];
        }

        size_t number_of_inputs() const {
            return inputs.size();
        }

        Module* get_input(size_t i) const {
            return inputs[i];
        }

        const string& get_label() const {
            return label;
        }

        int get_latched_value() const {
            return latched_value;
        }

        void dot(ostream& out) const {
            out << "digraph G {\n";
            unordered_set<const Module*> visited;
            dot(out, true, visited);
            visited.clear();
            dot(out, false, visited);
            out << "}\n";
        }

    private:

        void dot(ostream& out, bool only_nodes, unordered_set<const Module*>& visited) const {
            if (visited.count(this)) {
                return;
            }
            visited.insert(this);
            if (only_nodes) {
                string shape;
                switch (type) {
                    case broadcaster:
                        shape = "doubleoctagon";
                        break;
                    case flipflop:
                        shape = "box";
                        break;
                    case conjunction:
                        shape = "triangle";
                        break;
                    default:
                        assert(false);
                }
                out << label << " [shape=" << shape << "]\n";
            }
            else {
                for (auto* output : outputs) {
                    out << label << " -> " << output->get_label() << '\n';
                }
            }

            for (auto* output : outputs) {
                output->dot(out, only_nodes, visited);
            }
        }


        string label;
        Type type;
        vector<Module*> inputs;
        vector<Module*> outputs;
        int latched_value;

};


//struct Expression {
    //enum Type { literal, nand };
    //Type type;
    //Module* lit;
    //vector<Expression> operands;

    //bool expand() {
        //bool modified = false;
        //if (type == literal) {
            //if (lit->get_type() == Module::conjunction) {
                //operands.clear();
                //for (size_t i = 0; i < lit->number_of_inputs(); ++i) {
                    //operands.push_back(Expression{literal, lit->get_input(i), {}});
                //}
                //type = nand;
                //modified = true;
            //}
        //}
        //else {
            //for (Expression& op : operands) {
                //modified = modified || op.expand();
            //}
        //}
        //return modified;
    //}

    //vector<const Module*> literal_terms() const {
        //vector<const Module*> ans;
        //literal_terms(ans);
        //return ans;
    //}

    //void literal_terms(vector<const Module*>& out) const {
        //if (type == literal) {
            //out.push_back(lit);
        //}
        //for (const auto& exp : operands) {
            //exp.literal_terms(out);
        //}
    //}
//};

//ostream& operator<<(ostream& out, const Expression& exp) {
    //if (exp.type == Expression::literal) {
        //out << exp.lit->get_label();
    //}
    //else {
        //out << "nand(";
        //for (size_t i = 0; i < exp.operands.size(); ++i) {
            //if (i != 0) {
                //out << ", ";
            //}
            //out << exp.operands[i];
        //}
        //out << ')';
    //}
    //return out;
//}

Module* insert_or_modify(unordered_map<string,Module>& modules, const string& label, Module::Type type) {
    auto it = modules.find(label);
    if (it == modules.end()) {
        it = modules.try_emplace(label, label, type).first;
    }
    else {
        it->second.set_type(type);
    }
    return &(it->second);
}

Module* get_or_create(unordered_map<string,Module>& modules, const string& label) {
    auto it = modules.find(label);
    if (it == modules.end()) {
        it = modules.try_emplace(label, label).first;
    }
    return &(it->second);
}

unordered_map<string,Module> parse_modules(istream& in) {
    unordered_map<string,Module> modules;
    string line;
    while (getline(in, line)) {
        istringstream iss(line);
        string tx_label, rx_label, dummy;
        iss >> tx_label;
        Module* tx = nullptr;
        if (tx_label == "broadcaster") {
            tx = insert_or_modify(modules, tx_label, Module::broadcaster);
        }
        else if (tx_label[0] == '&') {
            tx = insert_or_modify(modules, tx_label.substr(1), Module::conjunction);
        }
        else if (tx_label[0] == '%') {
            tx = insert_or_modify(modules, tx_label.substr(1), Module::flipflop);
        }
        else {
            assert(false);
        }
        if (iss >> dummy) {
            assert(dummy=="->");
            while (iss >> rx_label) {
                if (rx_label.back() == ',') {
                    rx_label.pop_back();
                }
                tx->connect(get_or_create(modules, rx_label));
            }
        }
    }
    return modules;
}

ostream& operator<<(ostream& out, const Module& module) {
    switch (module.get_type()) {
        case Module::broadcaster:
            break;
        case Module::flipflop:
            out << '%';
            break;
        case Module::conjunction:
            out << '&';
            break;
        default:
            assert(false);
    }
    out << module.get_label();
    if (module.number_of_outputs()) {
        out << " -> ";
        out << module.get_output(0)->get_label();
        for (size_t i = 1; i < module.number_of_outputs(); ++i) {
            out << ", " << module.get_output(i)->get_label();
        }
    }
    return out;
}

void print_modules(const unordered_map<string,Module>& modules) {
    for (const auto&[label,module] : modules) {
        cout << module << endl;
    }
}

vector<const Module*> get_flipflops(const unordered_map<string,Module>& modules) {
    vector<const Module*> ret;
    for (const auto&[_,mod] : modules) {
        if (mod.get_type() == Module::flipflop) {
            ret.push_back(&mod);
        }
    }
    return ret;
}

size_t get_state(const vector<const Module*>& modules) {
    size_t state = 0;
    for (const auto* mod : modules) {
        state = state<<1 | mod->get_latched_value();
    }
    return state;
}

template<class Num>
size_t gcd(Num a, Num b) {
    while (b != Num(0)) {
        Num m = a%b;
        a = b;
        b = m;
    }
    return a;
}

template<class Num>
Num lcm(Num a, Num b) {
    return a/gcd(a,b)*b;
}

size_t time_to_rx(unordered_map<string,Module>& modules) {
    vector<size_t> periods;
    size_t remaining_periods = -1;
    for (size_t t = 1; remaining_periods; ++t) {
        queue<pair<Module*,int>> q;
        q.emplace(&(modules.find("broadcaster")->second), 0);
        while (!q.empty()) {
            auto[module,pulse] = q.front();
            q.pop();

            if (module->get_label()=="kc" && pulse==1) {
                if (periods.empty()) {
                    periods.resize(module->number_of_inputs());
                    remaining_periods = periods.size();
                }
                for (size_t i = 0; i < module->number_of_inputs(); ++i) {
                    int mod_out = module->get_input(i)->get_latched_value();
                    if (mod_out==1 && !periods[i]) {
                        periods[i] = t;
                        --remaining_periods;
                    }
                }
            }

            auto[output,echo] = module->ping(pulse);
            if (echo) {
                for (size_t i = 0; i < module->number_of_outputs(); ++i) {
                    if (output==0 && module->get_output(i)->get_label()=="rx") {
                        return t;
                    }
                    q.emplace(module->get_output(i), output);
                }
                
            }
        }
    }

    size_t ans = 1;
    for (size_t p : periods) {
        ans = lcm(ans, p);
    }

    return ans;
}

int main() {
    auto modules = parse_modules(cin);

    //modules.find("broadcaster")->second.dot(cout);

    //print_modules(modules);
    cout << time_to_rx(modules) << endl;
    
    //Expression exp{Expression::literal, &(modules.find("kc")->second), {}};
    //while (exp.expand());
    //cout << exp << endl;
}
