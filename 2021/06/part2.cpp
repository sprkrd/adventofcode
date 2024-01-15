#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

typedef uint64_t u64;

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

void step(vector<u64>& population) {
    u64 n_parents = population[0];
    for (int i = 1; i < 9; ++i) {
        population[i-1] = population[i];
    }
    population[6] += n_parents;
    population[8] = n_parents;
}

int main() {
    string line;
    getline(cin, line);
    vector<u64> population(9);
    for (const auto& phase : split(line, ",")) {
        u64 phase_int = stoull(phase);
        population[phase_int]++;
    }

    for (int i = 0; i < 256; ++i) {
        step(population);
    }

    u64 sum = 0;
    for (u64 fish : population) {
        sum += fish;
    }

    cout << sum << endl;
}
