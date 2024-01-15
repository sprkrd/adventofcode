#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
using namespace std;

struct Problem {
    string instructions;
    unordered_map<string, pair<string,string>> graph;

    void read(istream& in) {
        graph.clear();
        in >> instructions;
        string from;
        while (in >> from) {
            char dummy;
            string left, right;
            in >> dummy >> left >> right;
            assert(dummy == '=');
            left = left.substr(1, left.size()-2);
            right = right.substr(0, right.size()-1);
            graph.emplace(from, make_pair(left, right));
        }
    }

    size_t solve() const {
        size_t ans = 0; 
        size_t ins_pointer = 0;
        string current_node = "AAA";
        while (current_node != "ZZZ") {
            const auto& options = graph.find(current_node)->second;
            if (instructions[ins_pointer] == 'L') {
                current_node = options.first;
            }
            else {
                current_node = options.second;
            }
            if (++ins_pointer == instructions.size()) {
                ins_pointer = 0;
            }
            ++ans;
        }
        return ans;
    }
};

int main() {
    Problem problem;
    problem.read(cin);
    cout << problem.solve() << endl;
}
