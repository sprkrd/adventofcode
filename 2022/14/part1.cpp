#include <cassert>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
using namespace std;

typedef pair<int,int> Point2i;

const int k_sand_src_x = 500;
const int k_sand_src_y = 0;
const char k_wall = '#';
const char k_free = '.';
const char k_sand = 'o';

vector<int> every_int(const string& str) {
    static const regex rex("-?[0-9]+,?");
    vector<int> ans;
    sregex_iterator it(str.begin(), str.end(), rex);
    for (; it != sregex_iterator(); ++it) {
        ans.push_back(stoi(it->str()));
    }
    return ans;
}

struct Path {
    vector<Point2i> points;
};

ostream& operator<<(ostream& out, const Path& path) {
    bool first = true;
    for (auto[x,y] : path.points) {
        if (!first) out << " -> ";
        out << x << ',' << y;
        first = false;
    }
    return out;
}

vector<Path> read_input(istream& in) {
    vector<Path> ans;
    string line;
    while (getline(in, line)) {
        Path path;
        auto integers = every_int(line);
        assert(integers.size()%2 == 0);
        for (size_t i = 0; i < integers.size(); i += 2) {
            path.points.emplace_back(integers[i], integers[i+1]);
        }
        ans.push_back(move(path));
    }
    return ans;
}

struct Box {
    int xmin, xmax;
    int ymin, ymax;

    Box(int x, int y, int width, int height) : xmin(x), xmax(x+width-1),
                                               ymin(y), ymax(y+height-1) {}

    void fit(const Point2i& point) {
        auto[x,y] = point;
        xmin = min(xmin, x);
        xmax = max(xmax, x);
        ymin = min(ymin, y);
        ymax = max(ymax, y);
    }

    int width() const {
        return xmax - xmin + 1;
    }

    int height() const {
        return ymax - ymin + 1;
    }
};

int sign(int x) {
    if (!x) return 0;
    return x < 0? -1 : 1;
}

struct Map {
    string data;
    int width;
    int height;

    Map(int width = 0, int height = 0, char default_value = k_free) :
        data(width*height, default_value),
        width(width),
        height(height) {}

    int flat_index(int x, int y) const {
        return y*width + x;
    }

    char operator()(int x, int y) const {
        return data[flat_index(x,y)];
    }

    char& operator()(int x, int y) {
        return data[flat_index(x,y)];
    }

    void draw_segment(const Point2i& start, const Point2i& end, char value = k_wall) {
        auto[x_start,y_start] = start;
        auto[x_end,y_end] = end;
        int dx = sign(x_end - x_start);
        int dy = sign(y_end - y_start);
        assert(dx == 0 || dy == 0);
        while (x_start != x_end+dx) {
            (*this)(x_start,y_start) = value;
            x_start += dx;
        }
        while (y_start != y_end+dy) {
            (*this)(x_start,y_start) = value;
            y_start += dy;
        }
    }

    void draw(const Path& path, char value = k_wall) {
        for (size_t i = 1; i < path.points.size(); ++i) {
            draw_segment(path.points[i-1], path.points[i], value);
        }
    }

    bool within_bounds(int x, int y) const {
        return x >= 0 && x < width &&
               y >= 0 && y < height;
    }
};

struct System {
    enum Ministep_outcome { abyss, rest, falling };

    Map map;
    Box roi;
    Point2i sand_src;
    Point2i current_grain;
    int amount_of_sand;

    System(vector<Path> walls) : roi(k_sand_src_x,k_sand_src_y,1,1),
                                 sand_src(k_sand_src_x,k_sand_src_y),
                                 current_grain(-1, -1),
                                 amount_of_sand(0) {
        for (const Path& path : walls) {
            for (const Point2i& point : path.points) {
                roi.fit(point);
            }
        }
        --roi.xmin;
        ++roi.xmax;
        map = Map(roi.width(), roi.height());
        for (Path& path : walls) {
            for (auto&[x,y] : path.points) {
                x -= roi.xmin;
                y -= roi.ymin;
            }
            map.draw(path);
        }
        sand_src.first -= roi.xmin;
        sand_src.second -= roi.ymin;
    }

    Ministep_outcome ministep() {
        auto&[x_grain,y_grain] = current_grain;
        if (x_grain == -1) {
            current_grain = sand_src;
            return falling;
        }
        if (y_grain == map.height-1) {
            current_grain = Point2i(-1,-1);
            return abyss;
        }

        bool d_free = map(x_grain,y_grain+1) == k_free;
        bool dl_free = map(x_grain-1,y_grain+1) == k_free;
        bool dr_free = map(x_grain+1,y_grain+1) == k_free;
        bool moves = d_free||dl_free||dr_free;

        if (moves) {
            y_grain += 1;
            x_grain += (!d_free&&!dl_free) - (!d_free&&dl_free);
            return falling;
        }

        ++amount_of_sand;
        map(x_grain, y_grain) = k_sand;
        current_grain = Point2i(-1,-1);
        return rest;
    }

    bool step() {
        Ministep_outcome outcome;
        while ((outcome = ministep()) == falling);
        return outcome == rest;
    }
};

ostream& operator<<(ostream& out, const Map& map) {
    for (int y = 0; y < map.height; ++y) {
        for (int x = 0; x < map.width; ++x) {
            out << map(x,y);
        }
        out << '\n';
    }
    return out;
}

ostream& operator<<(ostream& out, const Box& box) {
    out << "Box @ (" << box.xmin << ',' << box.ymin
        << "), width=" << box.width() << ", height= " << box.height();
    return out;
}

int main() {
    vector<Path> walls = read_input(cin);
    System system(walls);
    //cout << system.map << endl;
    while (system.step()) {
        //cout << system.map << endl;
    }
    cout << system.map << endl;
    cout << system.amount_of_sand << endl;
}
