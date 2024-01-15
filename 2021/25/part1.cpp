#include <cassert>
#include <iostream>
using namespace std;

struct Map {
    string data;
    int width, height;

    void read(istream& in) {
        getline(in, data);
        width = data.length();
        height = 1;
        string line;
        while (getline(in, line)) {
            assert(line.length() == unsigned(width));
            ++height;
            data.insert(data.end(), line.begin(), line.end());
        }
    }

    char& operator()(int y, int x) {
        return data[y*width + x];
    }

    char operator()(int y, int x) const {
        return data[y*width + x];
    }

    bool step() {
        auto& map = *this;
        bool updated = false;
        // update cucumbers going east
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width;) {
                int x_east = x + 1;
                if (x_east == width)
                    x_east = 0;
                if (map(y,x) == '>' && map(y,x_east) == '.') {
                    map(y,x) = x == 0? 'g' : '.';
                    map(y,x_east) = '>';
                    x += 2;
                    updated = true;
                }
                else
                    x += 1;
            }
        }
        // remove "ghosts"
        for (int y = 0; y < height; ++y) {
            if (map(y,0) == 'g')
                map(y,0) = '.';
        }
        // update cucumbers going south
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height;) {
                int y_south = y + 1;
                if (y_south == height)
                    y_south = 0;
                if (map(y,x) == 'v' && map(y_south,x) == '.') {
                    map(y,x) = y == 0? 'g' : '.';
                    map(y_south,x) = 'v';
                    y += 2;
                    updated = true;
                }
                else
                    y += 1;
            }
        }
        // remove "ghosts"
        for (int x = 0; x < width; ++x) {
            if (map(0,x) == 'g')
                map(0,x) = '.';
        }
        return updated;
    }
};

ostream& operator<<(ostream& out, const Map& map) {
    for (int y = 0; y < map.height; ++y) {
        for (int x = 0; x < map.width; ++x) {
            out << map(y, x);
        }
        out << '\n';
    }
    return out;
}

int main() {
    Map map;
    map.read(cin);
    int step = 1;
    //cout << "Step " << step << endl;
    //cout << map << endl;
    while (map.step()) {
        ++step;
        //cout << "Step " << step << endl;
        //cout << map << endl;
    }
    cout << step << endl;
}

