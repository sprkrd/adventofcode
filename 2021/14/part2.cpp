#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

typedef uint64_t u64;
constexpr u64 inf = numeric_limits<u64>::max();

vector<string> split(const string& str, const string& delim=" \n\t") {
    vector<string> parts;
    size_t pos = 0;
    while (pos < str.length()) {
        size_t pos_next = str.find_first_of(delim, pos);
        if (pos_next > pos)
            parts.push_back(str.substr(pos, pos_next-pos));
        pos = pos_next == string::npos? string::npos : pos_next + 1;
    }
    return parts;
}

unordered_map<string,u64> expand_polymer(const unordered_map<string,u64>& compressed_polymer,
                                         const unordered_map<string,char>& rules) {

    unordered_map<string,u64> expanded_polymer;

    for (const auto&[chain,count] : compressed_polymer) {
        auto it = rules.find(chain);
        if (it == rules.end()) {
            expanded_polymer[chain] = count;
        }
        else {
            string child1{chain[0], it->second};
            string child2{it->second, chain[1]};
            expanded_polymer[child1] += count;
            expanded_polymer[child2] += count;
        }
    }

    return expanded_polymer;
}

int main() {
    unordered_map<string,char> rules;
    unordered_map<string,u64> compressed_polymer;
    string line, polymer;

    getline(cin, polymer);
    getline(cin, line);

    while (getline(cin, line)) {
        auto parts = split(line);
        rules[parts[0]] = parts[2][0];
    }

    for (int i = 1; i < polymer.size(); ++i) {
        string chain{polymer[i-1], polymer[i]};
        ++compressed_polymer[chain];
    }

    for (int i = 0; i < 40; ++i) {
        compressed_polymer = expand_polymer(compressed_polymer, rules);
    }
 
    unordered_map<char,u64> freqs;
    for (const auto&[chain,count] : compressed_polymer) {
        for (char c : chain) {
            freqs[c] += count;
        }
    }

    for (auto&[c,freq] : freqs) {
        u64 offset = (c==polymer.front()) + (c==polymer.back());
        freq = (freq + offset)/2;
    }

    for (auto[c,freq] : freqs) {
        cout << c << ": " << freq << endl;
    }

    u64 max_freq = 0;
    u64 min_freq = inf;

    for (auto[_,f] : freqs) {
        max_freq = max(max_freq, f);
        min_freq = min(min_freq, f);
    }

    u64 ans = max_freq - min_freq;
    cout << ans << endl;
}
