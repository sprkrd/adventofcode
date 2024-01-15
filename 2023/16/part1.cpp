#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <unordered_set>
using namespace std;


enum class Direction { north, west, south, east};

typedef pair<int,int> Vec2i;

typedef pair<Vec2i,Direction> BeamState;

Vec2i operator+(const Vec2i& l, const Vec2i& r) {
    return Vec2i(l.first+r.first, l.second+r.second);
}

Vec2i operator-(const Vec2i& l, const Vec2i& r) {
    return Vec2i(l.first-r.first, l.second-r.second);
}

Vec2i operator*(int s, const Vec2i& v) {
    return Vec2i(s*v.first, s*v.second);
}

Vec2i operator/(const Vec2i& v, int s) {
    return Vec2i(v.first/s, v.second/s);
}

Vec2i operator-(const Vec2i& v) {
    return Vec2i(-v.first, -v.second);
}

ostream& operator<<(ostream& out, const Vec2i& v) {
    return out << '(' << v.first << ", " << v.second << ')';
}

Vec2i dir2vec(Direction dir) {
    static constexpr Vec2i vectors[] = {{0,-1}, {-1,0}, {0,1}, {1,0}};
    return vectors[static_cast<int>(dir)];
}

Direction vec2dir(const Vec2i& v) {
    if (v==Vec2i(1,0)) {
        return Direction::east;
    }
    else if (v==Vec2i(-1,0)) {
        return Direction::west;
    }
    else if (v==Vec2i(0,1)) {
        return Direction::south;
    }
    else if (v==Vec2i(0,-1)) {
        return Direction::north;
    }
    assert(false);
}

struct BeamStateHasher {
    size_t operator()(const BeamState& beam_state) const noexcept {
        hash<int> int_hasher;
        size_t seed = int_hasher(beam_state.first.first);
        seed = seed*10'000'009 + int_hasher(beam_state.first.second);
        seed = seed*10'000'009 + int_hasher(static_cast<int>(beam_state.second));
        return seed;
    }
};


inline int dot(const Vec2i& u, const Vec2i& v) {
    return u.first*v.first + u.second*v.second;
}

inline bool orthogonal(const Vec2i& u, const Vec2i& v) {
    return dot(u,v) == 0;
}

Vec2i reflect(const Vec2i& d, const Vec2i& n) {
    return d -2*dot(d,n)*n/dot(n,n);
}


struct Map {
    int nrows;
    int ncols;
    string data;

    Map() {}

    Map(int nrows, int ncols, char def = '.') :
        nrows(nrows), ncols(ncols), data(nrows*ncols, def) {
    }

    int count(char c) const {
        return std::count(data.begin(), data.end(), c);
    }
    
    void read(istream& in) {
        nrows = 0;
        data.clear();
        string row;
        while (getline(in,row)) {
            ++nrows;
            ncols = row.size();
            data.insert(data.end(), row.begin(), row.end());        }
    }

    int ravel(const Vec2i& position) const {
        return position.second*ncols + position.first;
    }

    char at(const Vec2i& position) const {
        return data[ravel(position)];
    }

    char& at(const Vec2i& position) {
        return data[ravel(position)];
    }

    bool valid_position(const Vec2i& position) const {
        return 0<=position.first && position.first<ncols &&
               0<=position.second && position.second<nrows;
    }

    vector<BeamState> get_next_states(const BeamState& state) const {
        vector<BeamState> neighbors;
        auto[pos,dir] = state;
        Vec2i d = dir2vec(dir);
        if (at(pos)=='\\' || at(pos)=='/') {
            // reflect
            Vec2i n = at(pos)=='\\'? Vec2i(1,-1) : Vec2i(1,1);
            Vec2i reflection_vec = reflect(d, n);
            neighbors.emplace_back(pos+reflection_vec, vec2dir(reflection_vec));
        }
        else if (at(pos)=='|' || at(pos)=='-') {
            Vec2i wall_vec = at(pos)=='|'? Vec2i(0,1) : Vec2i(1,0);
            if (orthogonal(d, wall_vec)) {
                // split
                neighbors.emplace_back(pos+wall_vec, vec2dir(wall_vec));
                neighbors.emplace_back(pos-wall_vec, vec2dir(-wall_vec));
            }
            else {
                // pass through
                neighbors.emplace_back(pos+d, dir);
            }
        }
        else {
            // pass through
            neighbors.emplace_back(pos+d, dir);
        }

        // filter neighbors;
        for (auto it = neighbors.begin(); it != neighbors.end();) {
            if (!valid_position(it->first)) {
                it = neighbors.erase(it);
            }
            else {
                ++it;
            }
        }

        return neighbors;
    }
};

Map energize(const Map& input) {
    unordered_set<BeamState, BeamStateHasher> closed;
    vector<BeamState> stk{{{0,0},Direction::east}};
    while (!stk.empty()) {
        auto state = stk.back();
        stk.pop_back();
        if (closed.count(state)) {
            continue;
        }
        closed.insert(state);
        for (const auto& next : input.get_next_states(state)) {
            stk.push_back(next);
        }
    }
    Map out(input.nrows, input.ncols);
    for (const auto&[pos,_] : closed) {
        out.at(pos) = '#';
    }
    return out;
}



ostream& operator<<(ostream& out, const Map& map) {
    for (int y = 0; y < map.nrows; ++y) {
        for (int x = 0; x < map.ncols; ++x) {
            out << map.at(Vec2i(x,y));
        }
        out << '\n';
    }
    return out;
}


int main() {
    Map map;
    map.read(cin);
    //cout << map << endl;
    //cout << energize(map) << endl;
    cout << energize(map).count('#') << endl;
}
