#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
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

int eval(const vector<int>& f, int x) {
    int ans = 0;
    for (int c : f) {
        int diff = abs(c - x);
        ans += diff*(diff+1);
    }
    ans /= 2;
    return ans;
}

int main() {
    string line;
    getline(cin, line);
    auto parts = split(line, ",");
    
    vector<int> depths;
    depths.reserve(parts.size());
    for (const auto& depth_str : split(line, ",")) {
        depths.push_back(stoi(depth_str));
    }
    
    int previous_eval = eval(depths, 0);
    for (int x = 1;; ++x) {
        int current_eval = eval(depths, x);
        if (current_eval > previous_eval)
            break;
        previous_eval = current_eval;
    }
    
    cout << previous_eval << endl;
}
