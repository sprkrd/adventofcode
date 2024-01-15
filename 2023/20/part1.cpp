#include <cassert>
#include <iostream>
#include <queue>
#include <sstream>
#include <unordered_map>
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

        const string& get_label() const {
            return label;
        }

    private:

        string label;
        Type type;
        vector<Module*> inputs;
        vector<Module*> outputs;
        int latched_value;

};

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

size_t pulse_count(unordered_map<string,Module>& modules) {
    size_t low_pulses = 0;
    size_t high_pulses = 0;
    for (size_t t = 0; t < 1000; ++t) {
#if DEBUG
        cout << "button -low-> broadcaster" << endl;
#endif
        low_pulses += 1;

        queue<pair<Module*,int>> q;
        q.emplace(&(modules.find("broadcaster")->second), 0);
        while (!q.empty()) {
            auto[module,pulse] = q.front();
            q.pop();
            auto[output,echo] = module->ping(pulse);
            if (echo) {
                low_pulses += module->number_of_outputs()*(output==0);
                high_pulses += module->number_of_outputs()*(output==1);
                for (size_t i = 0; i < module->number_of_outputs(); ++i) {
#if DEBUG
                    cout << module->get_label() << " -"
                         << (output==0? "low" : "high") << "-> "
                         << module->get_output(i)->get_label() << endl;
#endif
                    q.emplace(module->get_output(i), output);
                }
            }
        }
    }
    return low_pulses*high_pulses;
}

int main() {
    auto modules = parse_modules(cin);
    //print_modules(modules);
    cout << pulse_count(modules) << endl;
}
