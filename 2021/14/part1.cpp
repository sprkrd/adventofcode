#include <cassert>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
using namespace std;

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

string expand_polymer(const string& polymer, const unordered_map<string,char>& rules) {
    string expanded_polymer;
    expanded_polymer.reserve(polymer.size()*2);

    char previous_char = '\0';

    string chain(2, ' ');
    for (char c : polymer) {
        chain[0] = previous_char;
        chain[1] = c;
        auto it = rules.find(chain);
        if (it != rules.end()) {
            expanded_polymer.push_back(it->second);
        }
        expanded_polymer.push_back(c);
        previous_char = c;
    }

    return expanded_polymer;
}

int main() {
    unordered_map<string,char> rules;
    string line, polymer;

    getline(cin, polymer);
    getline(cin, line);

    while (getline(cin, line)) {
        auto parts = split(line);
        rules[parts[0]] = parts[2][0];
    }

    for (int i = 0; i < 10; ++i) {
        polymer = expand_polymer(polymer, rules);
    }
    
    unordered_map<char, int> freqs;
    for (char c : polymer) {
        ++freqs[c];
    }

    int max_freq = 0;
    int min_freq = polymer.length() + 1;

    for (auto[_,f] : freqs) {
        max_freq = max(max_freq, f);
        min_freq = min(min_freq, f);
    }

    int ans = max_freq - min_freq;
    cout << ans << endl;
}
