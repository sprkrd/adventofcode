#include <cassert>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

size_t number_of_differences(const string& line1, const string& line2) {
    assert(line1.size() == line2.size());
    size_t ans = 0;
    for (size_t i = 0; i < line1.size(); ++i) {
        ans += line1[i] != line2[i];
    }
    return ans;
}

vector<string> transpose(const vector<string>& map) {
    vector<string> transposed(map.front().size(), string(map.size(), ' '));
    for (size_t i = 0; i < map.size(); ++i) {
        for (size_t j = 0; j < map[i].size(); ++j) {
            transposed[j][i] = map[i][j];
        }
    }
    return transposed;
}

size_t find_horizontal_reflection(const vector<string>& map) {
    for (size_t i = 1; i < map.size(); ++i) {
        size_t ndiffs = 0;
        size_t j_end = min(i, map.size()-i);
        size_t j = 0;
        while (j<j_end && ndiffs <= 1) {
            ndiffs += number_of_differences(map[i+j], map[i-1-j]);
            ++j;
        }
        if (j==j_end && ndiffs==1) {
            return i;
        }
    }
    return ~size_t(0);
}

size_t find_vertical_reflection(const vector<string>& map) {
    auto tmap = transpose(map);
    return find_horizontal_reflection(tmap);
}

size_t score(const vector<string>& map) {
    size_t href = find_horizontal_reflection(map);
    size_t vref = find_vertical_reflection(map);
    size_t ans = 0;
    if (href < ~(0ULL)) {
        ans += 100*href;
    }
    if (vref < ~(0ULL)) {
        ans += vref;
    }
    return ans;
}

vector<string> next_map(istream& in) {
    vector<string> map;
    string line;
    while (getline(in, line) && !line.empty()) {
        map.push_back(move(line));
    }
    return map;
}

ostream& operator<<(ostream& out, const vector<string>& map) {
    for (const auto& row : map) {
        out << row << '\n';
    }
    return out;
}

int main() {
    size_t sum = 0;
    auto map = next_map(cin);
    while (!map.empty()) {
        sum += score(map);
        map = next_map(cin);
    }
    cout << sum << endl;
}
