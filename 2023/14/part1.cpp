#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

enum class Direction { north, east, south, west };

inline bool isbetween(int x, int begin, int end) {
    return begin<=x && x<end;
}

inline int sign(int x) {
    return x==0? 0 : (x<0? -1 : 1);
}

void slide(vector<string>& platform, int x, int y, int dx, int dy) {
    int ncols = platform[0].size();
    int nrows = platform.size();
    while (isbetween(x+dx,0,ncols) && isbetween(y+dy,0,nrows) &&
           platform[y+dy][x+dx]=='.') {
        platform[y+dy][x+dx] = platform[y][x];
        platform[y][x] = '.';
        x += dx;
        y += dy;
    }
}

void tilt(vector<string>& platform, Direction dir) {
    int xbegin, xend, ybegin, yend, dx, dy, slide_dx, slide_dy;
    switch (dir) {
        case Direction::north:
            xbegin = 0;
            xend = platform[0].size();
            ybegin = 0;
            yend = platform.size();
            slide_dx = 0;
            slide_dy = -1;
            break;
        case Direction::east:
            xbegin = platform[0].size()-1;
            xend = -1;
            ybegin = 0;
            yend = platform.size();
            slide_dx = 1;
            slide_dy = 0;
            break;
        case Direction::south:
            xbegin = 0;
            xend = platform[0].size();
            ybegin = platform.size()-1;
            yend = -1;
            slide_dx = 0;
            slide_dy = 1;
            break;
        case Direction::west:
            xbegin = 0;
            xend = platform[0].size();
            ybegin = 0;
            yend = platform.size();
            slide_dx = -1;
            slide_dy = 0;
            break;
        default:
            assert(false);
    }
    dx = sign(xend-xbegin);
    dy = sign(yend-ybegin);
    for (int x = xbegin; x != xend; x += dx) {
        for (int y = ybegin; y != yend; y += dy) {
            if (platform[y][x] == 'O') {
                slide(platform, x, y, slide_dx, slide_dy);
            }
        }
    }
}

ostream& operator<<(ostream& out, const vector<string>& platform) {
    for (const auto& row : platform) {
        out << row << '\n';
    }
    return out;
}

int calculate_load(const vector<string>& platform) {
    int load = 0;
    int weight = platform.size();
    for (const auto& row : platform) {
        load += weight*count(row.begin(), row.end(), 'O');
        --weight;
    }
    return load;
}

int main() {
    vector<string> platform;
    string line;
    while (getline(cin, line)) {
        platform.push_back(move(line));
    }
    tilt(platform, Direction::north);
    cout << calculate_load(platform) << endl;
}
