#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Cargo {
    vector<string> configuration;

    void read(istream& in) {
        configuration.clear();
        string line;
        getline(cin, line);
        int nstacks = (line.length()-3)/4 + 1;
        configuration.resize(nstacks);
        while (in) {
            for (int stk_idx = 0; stk_idx < nstacks; ++stk_idx) {
                int line_idx = 4*stk_idx+1;
                if (line[line_idx] != ' ')
                    configuration[stk_idx].push_back(line[4*stk_idx+1]);
            }
            getline(in, line);
            assert(in);
            if (line[1] == '1')
                break;
        }
        for (string& stk : configuration)
            reverse(stk.begin(), stk.end());
    }

    void exec(int how_many, int from, int to) {
        configuration[to].insert(
                configuration[to].end(),
                configuration[from].rbegin(),
                configuration[from].rbegin()+how_many);
        configuration[from].erase(
                configuration[from].end()-how_many,
                configuration[from].end());
    }
};

ostream& operator<<(ostream& out, const Cargo& cargo) {
    int max_height = 0;
    for (const string& stk : cargo.configuration) {
        max_height = max(max_height, (int)stk.length());
    }
    int nstacks = cargo.configuration.size();
    for (int h = max_height-1; h >= 0; --h) {
        for (int stk_idx = 0; stk_idx < nstacks; ++stk_idx) {
            const string& stk = cargo.configuration[stk_idx];
            if (stk_idx > 0)
                out << ' ';
            if ((int)stk.length() > h)
                out << '[' << stk[h] << ']';
            else
                out << "   ";
        }
        out << '\n';
    }
    for (int stk_idx = 0; stk_idx < nstacks; ++stk_idx) {
        out << ' ';
        if (stk_idx > 0)
            out << "  ";
        out << (stk_idx+1);
    }
    out << '\n';
    return out;
}

int main() {
    Cargo cargo;
    cargo.read(cin);
    //cout << cargo << endl;
    string dummy;
    int how_many, from, to;
    while (cin >> dummy >> how_many >> dummy >> from >> dummy >> to) {
        cargo.exec(how_many, from-1, to-1);
        //cout << cargo << endl;
    }
    string ans;
    for (const auto& stk : cargo.configuration)
        ans.push_back(stk.back());
    cout << ans << endl;
}
