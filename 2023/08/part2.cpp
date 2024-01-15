#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
using namespace std;


typedef pair<string,size_t> State;

struct NavigationFunction {
    string instructions;
    unordered_map<string, pair<string,string>> graph;

    State operator()(const State& state) const {
        State ans;
        const auto& options = graph.find(state.first)->second;
        ans.first = instructions[state.second]=='L'? options.first : options.second;
        ans.second = state.second+1;
        if (ans.second == instructions.size()) {
            ans.second = 0;
        }
        return ans;
    }

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
};

pair<size_t,size_t> find_cycle(const NavigationFunction& f, const State& s) {
    State curr = s;
    size_t mu = 0;
    while (curr.first.back() != 'Z') {
        ++mu;
        curr = f(curr);
    }
    State cycle_start = curr;
    size_t lam = 1;
    curr = f(curr);
    while (curr != cycle_start) {
        assert(curr.first.back() != 'Z' && "this algorithm won't work here");
        ++lam;
        curr = f(curr);
    }
    assert(mu == lam && "oh no, the algorithm won't work :(");
    return {mu, lam};
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
Num mcm(Num a, Num b) {
    return a/gcd(a,b)*b;
}

int main() {
    NavigationFunction f;
    f.read(cin);
    vector<State> initial_states;
    for ([[maybe_unused]] const auto&[node,_] : f.graph) {
        if (node.back() == 'A') {
            initial_states.emplace_back(node, 0);
        }
    }

    size_t mcm_lam = 1;

    for (const auto& state : initial_states) {
        [[maybe_unused]] auto[_,lam] = find_cycle(f, state);
        mcm_lam = mcm(mcm_lam, lam);
    }

    size_t ans = mcm_lam;
    cout << ans << endl;
}
