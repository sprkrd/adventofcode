#include <array>
#include <cassert>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

typedef pair<int,int> Point2i;
typedef tuple<int,int,int> State;
enum class Direction { none, n, e, s, w };
const Direction all_directions[] {
    Direction::none, Direction::n, Direction::e, Direction::s, Direction::w };
constexpr char k_empty = 0;
constexpr char k_left_arrow = 1;
constexpr char k_right_arrow = 2;
constexpr char k_up_arrow = 4;
constexpr char k_down_arrow = 8;
constexpr char k_wall = 16;

ostream& operator<<(ostream& out, const Point2i& point) {
    return out << '(' << point.first << ',' << point.second << ')';
}

template <typename T>
inline void hash_combine(uint64_t& seed, const T& val) {
    // https://github.com/HowardHinnant/hash_append/issues/7
    std::hash<T> hasher;
    seed ^= hasher(val) + 0x9e3779b97f4a7c15 + (seed<<12) + (seed>>4); 
}

Point2i get_neighbor(const Point2i& point, Direction dir) {
    auto[x,y] = point;
    switch (dir) {
        case Direction::none:
            return point;
        case Direction::n:
            return Point2i(x, y-1);
        case Direction::e:
            return Point2i(x+1, y);
        case Direction::s:
            return Point2i(x, y+1);
        case Direction::w:
            return Point2i(x-1, y);
        default:
            assert(false);
    }
}

namespace std {
    template <>
    struct hash<State> {
        size_t operator()(const State& state) const {
            size_t seed = 0;
            hash_combine(seed, get<0>(state));
            hash_combine(seed, get<1>(state));
            hash_combine(seed, get<2>(state));
            return seed;
        }
    };
}

int wrap(int x, int lo, int hi) {
    int m = hi - lo + 1;
    return lo + (x - lo + m)%m;
}

struct Map {
    string data;
    int width;
    int height;
    
    char& at(int x, int y) {
        return data[y*width + x];
    }
    
    char at(int x, int y) const {
        return data[y*width + x];
    }
    
    bool operator==(const Map& other) const {
        return width == other.width && data == other.data;
    }
    
    bool operator!=(const Map& other) const {
        return !(*this == other);
    }
    
    Map next_configuration() {
        Map out(*this);
        for (int y = 1; y < height-1; ++y) {
            for (int x = 1; x < width-1; ++x) {
                int x_left = wrap(x-1, 1, width-2);
                int x_right = wrap(x+1, 1, width-2);
                int y_up = wrap(y-1, 1, height-2);
                int y_down = wrap(y+1, 1, height-2);
                out.at(x,y) = (at(x,y_up)&k_down_arrow) |
                              (at(x_left,y)&k_right_arrow) |
                              (at(x, y_down)&k_up_arrow) |
                              (at(x_right, y)&k_left_arrow);
            }
        }
        return out;
    }
    
    void read(istream& in) {
        data.clear();
        string line;
        while (getline(cin, line)) {
            width = line.length();
            for (char c : line) {
                char tr_c;
                switch (c) {
                    case '<':
                        tr_c = k_left_arrow;
                        break;
                    case '>':
                        tr_c = k_right_arrow;
                        break;
                    case '^':
                        tr_c = k_up_arrow;
                        break;
                    case 'v':
                        tr_c = k_down_arrow;
                        break;
                    case '.':
                        tr_c = k_empty;
                        break;
                    case '#':
                        tr_c = k_wall;
                        break;
                }
                data.push_back(tr_c);
            }
        }
        height = data.size() / width;
    }
};

ostream& operator<<(ostream& out, const Map& map) {
    for (int y = 0; y < map.height; ++y) {
        for (int x = 0; x < map.width; ++x) {
            char value = map.at(x, y);
            switch (value) {
                case k_empty:
                    out << '.';
                    break;
                case k_left_arrow:
                    out << '<';
                    break;
                case k_right_arrow:
                    out << '>';
                    break;
                case k_up_arrow:
                    out << '^';
                    break;
                case k_down_arrow:
                    out << 'v';
                    break;
                case k_wall:
                    out << '#';
                    break;
                default: {
                    int count = __builtin_popcount(value);
                    out << count;
                }
            }
        }
        out << '\n';
    }
    return out;
}

struct Environment {
    vector<Map> configurations;
    Point2i char_at;
    Point2i goal;
    int current_configuration;
    
    Map& map() {
        return configurations[current_configuration];
    }
    
    const Map& map() const {
        return configurations[current_configuration];
    }
    
    void init(istream& in) {
        Map start_configuration;
        start_configuration.read(in);
        int width = start_configuration.width;
        int idx_start = start_configuration.data.find_first_of(k_empty);
        int idx_goal = start_configuration.data.find_last_of(k_empty);
        char_at = Point2i(idx_start%width, idx_start/width);
        goal = Point2i(idx_goal%width, idx_goal/width);
        
        configurations.push_back(move(start_configuration));
        Map next = configurations.back().next_configuration();
        while (next != configurations.front()) {
            configurations.push_back(move(next));
            next = configurations.back().next_configuration();
        }
        current_configuration = 0;
    }
    
    State get_state() const {
        return State(char_at.first, char_at.second, current_configuration);
    }
    
    void set_state(const State& state) {
        tie(char_at.first, char_at.second, current_configuration) = state;
    }
    
    vector<Direction> get_available_actions() const {
        vector<Direction> available;
        int next_configuration = (current_configuration + 1)%configurations.size();
        const auto& next_map = configurations[next_configuration];
        for (auto dir : all_directions) {
            auto[x,y] = get_neighbor(char_at, dir);
            if (next_map.at(x, y) == k_empty) {
                available.push_back(dir);
            }
        }
        return available;
    }
    
    void step(Direction action) {
        char_at = get_neighbor(char_at, action);
        current_configuration = (current_configuration + 1)%configurations.size();
    }
    
    bool at_goal() const {
        return char_at == goal;
    }
    
    int blizzard_period() const {
        return configurations.size();
    }
};

ostream& operator<<(ostream& out, const Environment& env) {
    out << env.map() << '\n'
        << env.char_at << '\n'
        << env.goal << '\n';
    return out;
}

ostream& operator<<(ostream& out, const State& state) {
    out << '(' << get<0>(state) << ','
        << get<1>(state) << ','
        << get<2>(state) << ')';
    return out;
}

int bfs(Environment& env) {
    unordered_set<State> closed;
    queue<pair<State,int>> open;
    open.emplace(env.get_state(), 0);
    while (!open.empty()) {
        auto[state, level] = open.front();
        open.pop();
        if (closed.count(state)) {
            continue;
        }
        closed.insert(state);
        env.set_state(state);
        if (env.at_goal()) {
            return level;
        }
        for (auto action : env.get_available_actions()) {
            env.step(action);
            open.emplace(env.get_state(), level+1);
            env.set_state(state);
        }
    }
    return -1;
}

int main() {
    Environment env;
    env.init(cin);
    //cout << env << endl;
    cout << bfs(env) << endl;
}
