#include <cassert>
#include <iostream>
#include <string>
#include <vector>
using namespace std;


struct Screen {
    vector<string> data;

    Screen(int width, int height) : data(height,string(width,' ')) {

    }

    char& operator()(int i, int j) {
        return data[i][j];
    }

    void draw(int cycle, int x) {
        uint64_t mask = x>=1? 0x7ULL<<(x-1) : 0x7ULL>>(1-x);
        int width = data.front().length();
        int i = cycle/width;
        int j = cycle%width;
        if ((1ULL<<j) & mask) {
            (*this)(i,j) = '#';
        }
    }
};

ostream& operator<<(ostream& out, const Screen& screen) {
    for (const string& row : screen.data) {
        out << row << '\n';
    }
    return out;
}


int main() {
    Screen screen(40, 6);
    int cycle = 0;
    int x = 1;
    string cmd;
    while (cin >> cmd) {
        screen.draw(cycle, x);
        if (cmd == "addx") {
            int arg;
            cin >> arg;
            screen.draw(++cycle, x);
            x += arg;
        }
        ++cycle;
    }
    cout << screen << endl;
}
