#include <array>
#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
using namespace std;

typedef pair<int,int> Point2i;
constexpr int num_directions = 8;
enum class Direction { n, ne, e, se, s, sw, w, nw };

template <typename T>
inline void hash_combine(uint64_t& seed, const T& val) {
    // https://github.com/HowardHinnant/hash_append/issues/7
    std::hash<T> hasher;
    seed ^= hasher(val) + 0x9e3779b97f4a7c15 + (seed<<12) + (seed>>4); 
}

Point2i get_neighbor(const Point2i& point, Direction dir) {
    auto[x,y] = point;
    switch (dir) {
        case Direction::n:
            return Point2i(x, y-1);
        case Direction::ne:
            return Point2i(x+1, y-1);
        case Direction::e:
            return Point2i(x+1, y);
        case Direction::se:
            return Point2i(x+1, y+1);
        case Direction::s:
            return Point2i(x, y+1);
        case Direction::sw:
            return Point2i(x-1, y+1);
        case Direction::w:
            return Point2i(x-1, y);
        case Direction::nw:
            return Point2i(x-1, y-1);
        default:
            assert(false);
    }
}

namespace std {
    template <>
    struct hash<Point2i> {
        size_t operator()(const Point2i& point) const {
            size_t seed = 0;
            hash_combine(seed, point.first);
            hash_combine(seed, point.second);
            return seed;
        }
    };
}

array<Direction,4> directions_to_check{
    Direction::n,
    Direction::s,
    Direction::w,
    Direction::e
};

struct Environment {
    unordered_set<Point2i> elves;
    int current_dir_index;
    
    void init(istream& in) {
        elves.clear();
        current_dir_index = 0;
        int y = 0;
        string line;
        while (getline(cin, line) && !line.empty()) {
            for (int x = 0; x < (int)line.length(); ++x) {
                if (line[x] == '#') {
                    elves.emplace(x, y);
                }
            }
            ++y;
        }
    }
    
    bool check_direction(const Point2i& point, Direction dir) {
        for (int i = -1; i <= 1; ++i) {
            Direction adj_dir = static_cast<Direction>(
                (static_cast<int>(dir) + i + num_directions)%num_directions);
            auto neigh = get_neighbor(point, adj_dir);
            if (elves.count(neigh)) {
                return false;
            }
        }
        return true;
    }
    
    bool check_isolated(const Point2i& point) {
        for (int i = 0; i < num_directions; ++i) {
            Direction dir = static_cast<Direction>(i);
            auto neigh = get_neighbor(point, dir);
            if (elves.count(neigh)) {
                return false;
            }
        }
        return true;
    }
    
    void step() {
        unordered_map<Point2i,Point2i> proposed_locations;
        unordered_map<Point2i,int> num_proposals;
        
        for (const auto& point : elves) {
            if (check_isolated(point)) {
                continue;
            }
            for (int i = 0; i < (int)directions_to_check.size(); ++i) {
                int dir_index = (current_dir_index + i)%directions_to_check.size();
                Direction dir = directions_to_check[dir_index];
                if (!check_direction(point, dir)) {
                    continue;
                } 
                auto neighbor = get_neighbor(point, dir);
                proposed_locations[point] = neighbor;
                ++num_proposals[neighbor];
                break;
            }
        }
        
        unordered_set<Point2i> new_point_set;
        for (const auto& point : elves) {
            auto it = proposed_locations.find(point);
            int num_proposals_point = 0;
            if (it != proposed_locations.end()) {
                num_proposals_point = num_proposals.at(it->second);
            }
            if (num_proposals_point == 1) {
                new_point_set.insert(it->second);
            }
            else {
                new_point_set.insert(point);
            }
        }
        elves = move(new_point_set);
        current_dir_index = (current_dir_index + 1) % directions_to_check.size();
    }
};

struct Box {
    int x_min;
    int x_max;
    int y_min;
    int y_max;
    
    void fit(const unordered_set<Point2i>& points) {
        x_min = y_min = numeric_limits<int>::max();
        x_max = y_max = numeric_limits<int>::min();
        for (const auto& [x,y] : points) {
            x_min = min(x, x_min);
            x_max = max(x, x_max);
            y_min = min(y, y_min);
            y_max = max(y, y_max);
        }
    }
    
    int width() const {
        return x_max - x_min + 1;
    }
    
    int height() const {
        return y_max - y_min + 1;
    }
    
    int area() const {
        return width()*height();
    }
};

ostream& operator<<(ostream& out, const Environment& env) {
    Box box;
    box.fit(env.elves);
    for (int y = 0; y < box.height(); ++y) {
        for (int x = 0; x < box.width(); ++x) {
            Point2i point(box.x_min + x, box.y_min + y);
            out << (env.elves.count(point)? '#' : '.');
        }
        out << '\n';
    }
    return out;
}

int main() {
    Environment env;
    env.init(cin);
    //cout << env << endl;
    for (int i = 0; i < 10; ++i) {
        env.step();
        //cout << env << endl;
    }
    Box box;
    box.fit(env.elves);
    int ans = box.area() - env.elves.size();
    cout << ans << endl;
}
