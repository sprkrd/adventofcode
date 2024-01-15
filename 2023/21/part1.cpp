#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
using namespace std;

typedef pair<int,int> Vec2i;

enum class Direction { north, west, south, east};

constexpr Direction all_directions[] = {
    Direction::north,
    Direction::west,
    Direction::south,
    Direction::east
};

Vec2i dir2vec(Direction dir) {
    static constexpr Vec2i vectors[] = {{0,-1}, {-1,0}, {0,1}, {1,0}};
    return vectors[static_cast<int>(dir)];
}

Vec2i operator+(const Vec2i& l, const Vec2i& r) {
    return Vec2i(l.first+r.first, l.second+r.second);
}

template <class Num>
bool isbetween(Num x, Num lo, Num hi) {
    return lo<=x && x<=hi;
}

struct Map {
    int nrows;
    int ncols;
    string data;

    Map() {}

    Map(int nrows, int ncols, char def = '.') :
        nrows(nrows), ncols(ncols), data(nrows*ncols, def) {
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

    Vec2i start() const {
        int loc = data.find_first_of('S');
        return unravel(loc);
    }

    int ravel(const Vec2i& position) const {
        return position.second*ncols + position.first;
    }

    Vec2i unravel(int loc) const {
        return {loc%ncols, loc/ncols};
    }

    char at(const Vec2i& position) const {
        return data[ravel(position)];
    }

    char& at(const Vec2i& position) {
        return data[ravel(position)];
    }

    vector<pair<Direction,Vec2i>> get_neighbors(const Vec2i& position) const {
        vector<pair<Direction,Vec2i>> neighbors;
        for (auto dir : all_directions) {
            Vec2i neighbor = position + dir2vec(dir);
            if (isbetween(neighbor.first,0,ncols-1) &&
                isbetween(neighbor.second,0,nrows-1) &&
                at(neighbor) != '#') {
                neighbors.emplace_back(dir, neighbor);
            }
        }
        return neighbors;
    }
};

size_t reachable_plots(const Map& map, size_t steps) {
    set<pair<size_t,Vec2i>> current_set{{0,map.start()}};
    while (current_set.begin()->first != steps) {
        auto[level,position] = *current_set.begin();
        current_set.erase(current_set.begin());
        for (auto[dir,neighbor] : map.get_neighbors(position)) {
            current_set.emplace(level+1,neighbor);
        }
    }
    return current_set.size();
}

int main() {
    Map map;
    map.read(cin);
    cout << reachable_plots(map, 64) << endl;
}
