#include <cctype>
#include <iostream>
#include <string>
#include <vector>
using namespace std;


struct Part {
    int id;
    int row;
    int col_begin;
    int col_end;
};

Part get_part(const vector<string>& engine, int row, int col) {
    if (!isdigit(engine[row][col])) {
        return {-1, row, col+1, col+1};
    }
    int ncols = engine[row].size();
    int col_begin = col;
    while (col_begin>0 && isdigit(engine[row][col_begin-1])) {
        --col_begin;
    }
    int col_end = col + 1;
    while (col_end<ncols && isdigit(engine[row][col_end])) {
        ++col_end;
    }
    int id = stoi(engine[row].substr(col_begin, col_end-col_begin));
    return {id, row, col_begin, col_end};
}

vector<Part> adjacent_parts(const vector<string>& engine, int row, int col) {
    vector<Part> parts;
    int nrows = engine.size();
    int ncols = engine[row].size();
    for (int r = max(0, row-1); r < min(nrows, row+2); ++r) {
        for (int c = max(0, col-1); c < min(ncols, col+2);) {
            auto part = get_part(engine, r, c);
            if (part.id == -1) {
                ++c;
            }
            else {
                parts.push_back(part);
                c = part.col_end;
            }
        }
    }
    return parts;
}

int gear_ratio(const vector<string>& engine) {
    int sum = 0;
    for (int row = 0; row < engine.size(); ++row) {
        for (int col = 0; col < engine[row].size(); ++col) {
            if (engine[row][col] == '*') {
                auto parts = adjacent_parts(engine, row, col);
                if (parts.size() == 2) {
                    sum += parts[0].id * parts[1].id;
                }
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
    cout << gear_ratio(engine) << endl;
}
