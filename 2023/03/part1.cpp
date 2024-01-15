#include <cctype>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

bool is_symbol(char c) {
    return c != '.' && !isdigit(c);
}

bool is_part(const vector<string>& engine, int row, int begin, int end) {
    int ncols = engine[0].size();
    if (row > 0) {
        for (int col = max(0, begin-1); col < min(ncols, end+1); ++col) {
            if (is_symbol(engine[row-1][col])) {
                return true;
            }
        }
    }
    if (begin>0 && is_symbol(engine[row][begin-1])) {
        return true;
    }
    if (end<ncols && is_symbol(engine[row][end])) {
        return true;
    }
    if (row < engine.size()-1) {
        for (int col = max(0, begin-1); col < min(ncols, end+1); ++col) {
            if (is_symbol(engine[row+1][col])) {
                return true;
            }
        }
    }
    return false;
}

int get_part_sum(const vector<string>& engine) {
    int sum = 0;
    for (int row = 0; row < engine.size(); ++row) {
        for (int col = 0; col < engine[row].size();) {
            if (isdigit(engine[row][col])) {
                int end = col+1;
                while (end<engine.size() && isdigit(engine[row][end])) {
                    ++end;
                }
                int num = stoi(engine[row].substr(col, end-col));
                sum += is_part(engine, row, col, end)*num;
                col = end;
            }
            else {
                ++col; 
            }
        }
    }
    return sum;
}

int main() {
    vector<string> engine;
    string line;
    while (getline(cin, line)) {
        engine.push_back(move(line));
    }
    cout << get_part_sum(engine) << endl;
}
