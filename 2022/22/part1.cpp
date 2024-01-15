#include <algorithm>
#include <iostream>
#include <string>
#include <variant>
#include <vector>
using namespace std;

typedef pair<int,int> Point;

ostream& operator<<(ostream& out, const Point& point) {
    auto[x,y] = point;
    return out << '(' << x << ',' << y << ')';
}

constexpr char k_wall = '#';
constexpr char k_empty = '.';
constexpr char k_eor = ' ';
constexpr char k_character = '@';

constexpr int k_num_directions = 4;
constexpr int k_right = 0;
constexpr int k_down = 1;
constexpr int k_left = 2;
constexpr int k_up = 3;

constexpr Point direction_vectors[] { {1,0}, {0,1}, {-1,0}, {0,-1} };
const char* direction_symbols = ">v<^";

inline int wrap(int x, int m) {
    return (x%m + m)%m;
}

inline int wrap(int x, int lo, int hi) {
    return lo + wrap(x - lo, hi - lo + 1);
}

struct Map {
    struct Row {
        int x_start;
        int x_end;
        string row_data;

        Row(string line) : row_data(move(line)) {
            x_start = row_data.find_first_not_of(k_eor);
            x_end = row_data.find_last_not_of(k_eor);
        }

        char at(int x) const {
            return row_data.at(x);
        }

        char& at(int x) {
            return row_data.at(x);
        }

        int width() const {
            return row_data.length();
        }
    };

    struct Column_limits {
        int y_start;
        int y_end;
    };

    vector<Row> data;
    vector<Column_limits> column_limits;
    Point char_at;
    int direction;

    int height() const {
        return data.size();
    }

    int width(int y) const {
        return data.at(y).width();
    }

    void read(istream& in) {
        string line;
        while (getline(cin, line) && !line.empty()) {
            int y = data.size();
            data.emplace_back(move(line));
            if (width(y) > column_limits.size()) {
                column_limits.resize(width(y), {-1,-1});
            }
            for (int x = data[y].x_start; x <= data[y].x_end; ++x) {
                int& y_start = column_limits[x].y_start;
                int& y_end = column_limits[x].y_end;
                if (data[y].at(x) != k_eor) {
                    if (y_start == -1) {
                        y_start = y;
                    }
                    y_end = y;
                }
            }
        }
        auto[x,y] = char_at = find_starting_position();
        (*this)(x,y) = k_character;
        direction = k_right;
    }

    Point find_starting_position() const {
        for (int y = 0; y < height(); ++y) {
            auto it = find(data[y].row_data.begin(),
                           data[y].row_data.end(),
                           k_empty);
            if (it != data[y].row_data.end()) {
                int x = it - data[y].row_data.begin();
                return {x, y};
            }
        }
        return {-1,-1};
    }

    char operator()(int x, int y) const {
        return data.at(y).at(x);
    }

    char& operator()(int x, int y) {
        return data.at(y).at(x);
    }

    void turn(char turn_cmd) {
        int dir_increment = turn_cmd == 'R'? 1 : -1;
        direction = wrap(direction+dir_increment, k_num_directions);
    }

    void forward(int steps) {
        auto[x,y] = char_at;
        auto[dx,dy] = direction_vectors[direction];
        while (steps) {
            (*this)(x,y) = direction_symbols[direction];
            int x_next = wrap(x + dx, data.at(y).x_start,
                                      data.at(y).x_end);
            int y_next = wrap(y + dy, column_limits.at(x).y_start,
                                      column_limits.at(x).y_end);
            if ((*this)(x_next,y_next) != k_wall) {
                x = x_next;
                y = y_next;
                --steps;
            }
            else {
                steps = 0;
            }
        }
        char_at = Point(x,y);
        (*this)(x, y) = k_character;
    }
};

ostream& operator<<(ostream& out, const Map& map) {
    for (int y = 0; y < map.height(); ++y) {
        for (int x = 0; x < map.width(y); ++x) {
            out << map(x, y);
        }
        out << '\n';
    }
    return out;
}

typedef variant<int,char> Cmd;

vector<Cmd> parse_commands(const string& line) {
    vector<Cmd> commands;
    size_t pos = 0;
    while (pos < line.length()) {
        if (line[pos] == 'R' || line[pos] == 'L') {
            commands.emplace_back(line[pos++]);
        }
        else {
            size_t next_pos = line.find_first_of("LR", pos);
            size_t count = next_pos - pos;
            commands.emplace_back(stoi(line.substr(pos, count)));
            pos = next_pos;
        }
    }
    return commands;
}

struct Cmd_printer {
    ostream& out;

    Cmd_printer(ostream& out) : out(out) {}

    template <typename T>
    void operator()(T item) const {
        out << item;
    }
};

struct Cmd_exec {
    Map& map;

    Cmd_exec(Map& map) : map(map) {}

    void operator()(int steps) const {
        map.forward(steps);
    }

    void operator()(char turn_cmd) const {
        map.turn(turn_cmd);
    }
};

int main() {
    Map map;
    map.read(cin);
    string instructions;
    cin >> instructions;

    Cmd_exec exec(map);
    for (auto&& cmd : parse_commands(instructions)) {
        visit(exec, cmd);
    }

    //map.forward(10);
    //map.turn('R');
    //map.forward(5);
    //map.turn('L');
    //map.forward(5);
    //map.turn('R');
    //map.forward(10);
    //map.turn('L');
    //map.forward(4);
    //map.turn('R');
    //map.forward(5);
    //map.turn('L');
    //map.forward(5);

    cout << map << endl;

    auto[x,y] = map.char_at;
    int row = y + 1;
    int col = x + 1;
    int dir = map.direction;

    int ans = 1000*row + 4*col + dir;

    cout << ans << endl;
}
