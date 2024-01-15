#include <iostream>
using namespace std;

struct Map {
    string data;
    int width, height;

    Map() {}

    Map(int width, int height, int default_value = 0) :
        data(width*height, '0'+default_value),
        width(width),
        height(height) {}

    void read(istream& in) {
        getline(in, data);
        width = data.length();
        height = 1;
        string line;
        while (getline(in, line)) {
            ++height;
            data.insert(data.end(), line.begin(), line.end());
        }
    }

    int get(int i, int j) const {
        return data[i*width+j] - '0';
    }

    void set(int i, int j, int value) {
        data[i*width+j] = '0' + value;
    }
};

ostream& operator<<(ostream& out, const Map& map) {
    for (int i = 0; i < map.height; ++i) {
        for (int j = 0; j < map.width; ++j) {
            out << map.get(i, j);
        }
        out << '\n';
    }
    return out;
}

int main() {
    Map map;
    map.read(cin);

    int w = map.width;
    int h = map.height;

    Map acc_left(w, h);
    for (int i = 0; i < h; ++i) {
        acc_left.set(i, 0, map.get(i,0));
        for (int j = 1; j < w; ++j) {
            int value = max(map.get(i,j), acc_left.get(i,j-1));
            acc_left.set(i, j, value); 
        }
    }

    Map acc_right(w, h);
    for (int i = 0; i < h; ++i) {
        acc_right.set(i, w-1, map.get(i,w-1));
        for (int j = w-2; j >= 0; --j) {
            int value = max(map.get(i,j), acc_right.get(i,j+1));
            acc_right.set(i, j, value); 
        }
    }

    Map acc_top(w, h);
    for (int j = 0; j < w; ++j) {
        acc_top.set(0, j, map.get(0, j));
        for (int i = 1; i < h; ++i) {
            int value = max(map.get(i,j), acc_top.get(i-1,j));
            acc_top.set(i, j, value);
        }
    }

    Map acc_bottom(w, h);
    for (int j = 0; j < w; ++j) {
        acc_bottom.set(h-1, j, map.get(h-1, j));
        for (int i = h-2; i >= 0; --i) {
            int value = max(map.get(i,j), acc_bottom.get(i+1,j));
            acc_bottom.set(i, j, value);
        }
    }

    int visible = 2*w + 2*h - 4;
    for (int i = 1; i < h-1; ++i) {
        for (int j = 1; j < w-1; ++j) {
            visible +=
                map.get(i,j) > acc_left.get(i,j-1) ||
                map.get(i,j) > acc_right.get(i,j+1) ||
                map.get(i,j) > acc_top.get(i-1,j) ||
                map.get(i,j) > acc_bottom.get(i+1,j);

        }
    }
    cout << visible << endl;
}
