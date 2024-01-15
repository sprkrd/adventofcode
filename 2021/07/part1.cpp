#include <algorithm>
#include <cassert>
#include <cmath>
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

double median(const vector<int>& x) {
    auto x_sorted = x;
    sort(x_sorted.begin(), x_sorted.end());
    if (x_sorted.size()%2 == 0) {
        return 0.5*(x_sorted[x_sorted.size()/2-1] + x_sorted[x_sorted.size()/2]);
    }
    return x_sorted[x_sorted.size()/2];
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
    
    double med = median(depths);
    
    double fuel = 0;
    
    for (int d : depths) {
        fuel += fabs(d - med);
    }
    
    cout << fuel << endl;
}
