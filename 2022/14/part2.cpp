#include <curses.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <thread>
#include <vector>
using namespace std;

typedef pair<int,int> Point2i;

const int k_sand_src_x = 500;
const int k_sand_src_y = 0;
const char k_wall = '#';
const char k_free = ' ';
const char k_sand = '.';

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

struct SparseMap {
    map<Point2i,char> data;
    int floor_level;

    SparseMap(int floor_level = 0) : floor_level(floor_level) {}

    char at(int x, int y) const {
        Point2i point(x,y);
        auto it = data.find(point);
        if (it == data.end()) {
            return y >= floor_level? k_wall : k_free;
        }
        return it->second;
    }

    void set(int x, int y, char value) {
        Point2i point(x,y);
        if ((y < floor_level && value == k_free) ||
            (y >= floor_level && value == k_wall)) {
            data.erase(point);
        }
        data[point] = value;
    }

    void draw_segment(const Point2i& start, const Point2i& end, char value = k_wall) {
        auto[x_start,y_start] = start;
        auto[x_end,y_end] = end;
        int dx = sign(x_end - x_start);
        int dy = sign(y_end - y_start);
        assert(dx == 0 || dy == 0);
        while (x_start != x_end+dx) {
            set(x_start,y_start,value);
            x_start += dx;
        }
        while (y_start != y_end+dy) {
            set(x_start,y_start,value);
            y_start += dy;
        }
    }

    void draw(const Path& path, char value = k_wall) {
        for (size_t i = 1; i < path.points.size(); ++i) {
            draw_segment(path.points[i-1], path.points[i], value);
        }
    }

    Box get_roi() const {
        Box roi(k_sand_src_x, k_sand_src_y,
                1, floor_level-k_sand_src_y+1);
        for (const auto&[point,_] : data) {
            roi.fit(point);
        }
        return roi;
    }
};

struct System {
    enum Ministep_outcome { rest, falling, source_blocked };

    SparseMap map;
    Point2i current_grain;
    int amount_of_sand;

    System(const vector<Path>& walls) : current_grain(-1, -1),
                                        amount_of_sand(0) {
        int floor_level = 0;
        for (const Path& path : walls) {
            for (auto&&[x,y] : path.points) {
                floor_level = max(floor_level, y+2);
            }
        }
        map.floor_level = floor_level;
        for (const Path& path : walls) {
            map.draw(path);
        }
    }

    Ministep_outcome ministep() {
        auto&[x_grain,y_grain] = current_grain;
        if (x_grain == -1) {
            if (map.at(k_sand_src_x,k_sand_src_y) != k_free) {
                return source_blocked;
            }
            x_grain = k_sand_src_x;
            y_grain = k_sand_src_y;
            return falling;
        }

        bool d_free = map.at(x_grain,y_grain+1) == k_free;
        bool dl_free = map.at(x_grain-1,y_grain+1) == k_free;
        bool dr_free = map.at(x_grain+1,y_grain+1) == k_free;
        bool moves = d_free||dl_free||dr_free;

        if (moves) {
            y_grain += 1;
            x_grain += (!d_free&&!dl_free) - (!d_free&&dl_free);
            return falling;
        }

        ++amount_of_sand;
        map.set(x_grain, y_grain, k_sand);
        current_grain = Point2i(-1,-1);
        return rest;
    }

    bool step() {
        Ministep_outcome outcome;
        while ((outcome = ministep()) == falling);
        return outcome == rest;
    }
};

ostream& operator<<(ostream& out, const SparseMap& map) {
    auto roi = map.get_roi();
    for (int y = roi.ymin; y <= roi.ymax; ++y) {
        for (int x = roi.xmin; x <= roi.xmax; ++x) {
            out << map.at(x, y);
        }
        out << '\n';
    }
    return out;
}

void output_system(WINDOW* win, const System& system, const Box& roi) {
    wmove(win,0,0);
    for (int y = 0; y < roi.height(); ++y) {
        for (int x = 0; x < roi.width(); ++x) {
            waddch(win, system.map.at(roi.xmin+x,roi.ymin+y));
        }
    }
    mvwaddch(win, k_sand_src_y-roi.ymin, k_sand_src_x-roi.xmin,'+');
    auto[x_grain,y_grain] = system.current_grain;
    if (x_grain != -1) {
        mvwaddch(win, y_grain-roi.ymin, x_grain-roi.xmin, k_sand);
    }
    mvwprintw(win, roi.height(), 0, "Sand: %d", system.amount_of_sand);
    wrefresh(win);
}



int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " input_filename\n";
        return 1;
    }
    ifstream in(argv[1]);
    if (!in) {
        cerr << "Couldn't read from " << argv[1] << '\n';
        return 1;
    }
    vector<Path> walls = read_input(in);
    System system(walls);

    auto roi = system.map.get_roi();

    initscr();
    cbreak();
    noecho();
    WINDOW* win = newwin(roi.height()+1, roi.width(), 0, 0);
    wrefresh(win);
    output_system(win, system, roi);
    //cout << system.map << endl;
    while (system.ministep() != System::source_blocked) {
        this_thread::sleep_for(chrono::milliseconds(16));
        output_system(win, system, roi);
        //cout << system.map << endl;
    }
    wgetch(win);
    delwin(win);
    endwin();
}
