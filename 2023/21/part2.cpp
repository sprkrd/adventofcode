#include <cassert>
#include <iostream>
#include <limits>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <valarray>
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

    char at(int x, int y) const {
        return data[ravel({x,y})];
    }

    char& at(int x, int y) {
        return data[ravel({x,y})];
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

template<class Num>
struct Matrix {
    size_t nrows;
    size_t ncols;
    valarray<Num> data;

    Matrix(size_t nrows, size_t ncols, Num default_val = Num(0)) :
        nrows(nrows), ncols(ncols), data(default_val, nrows*ncols) {}

    Matrix(size_t ncols, valarray<Num> data) :
        nrows(data.size()/ncols), ncols(ncols), data(move(data)) {}

    Num at(size_t i, size_t j) const {
        return data[i*ncols + j];
    }

    Num& at(size_t i, size_t j) {
        return data[i*ncols + j];
    }

    Matrix<bool> operator==(const Matrix<Num>& other) const {
        return Matrix<bool>(ncols, data==other.data);
    }
};

Matrix<int> distance_transform(const Map& map, const Vec2i& start) {
    auto[x_start,y_start] = start;

    Matrix<int> dist(map.nrows, map.ncols, numeric_limits<int>::min());

    dist.at(y_start, x_start) = 0;

    queue<Vec2i> open;
    open.emplace(x_start, y_start);

    while (!open.empty()) {
        auto[x,y] = open.front();
        open.pop();
        
        if (dist.at(y, x) > 0) {
            break;
        }

        dist.at(y, x) = -dist.at(y, x);
        for (auto[dir,neighbor] : map.get_neighbors(Vec2i(x,y))) {
            auto[nx,ny] = neighbor;
            int g = dist.at(y,x) + 1;
            if (-g>dist.at(ny,nx)) {
                dist.at(ny,nx) = -g;
                open.emplace(nx,ny);
            }
        }
    }
    return dist;
}

int manhattan_distance(const Vec2i& a, const Vec2i& b) {
    return abs(a.first-b.first) + abs(a.second-b.second);
}

uint64_t sum_of_odds(uint64_t N) {
    // N: the number of odd numbers
    return N*N;
}

uint64_t sum_of_evens(uint64_t N) {
    // N: the number of even numbers
    return N*(N+1);
}

uint64_t reachable_plots_origin(const Map& map, int steps) {
    // original world
    //     .
    //    ...
    //   .....
    //  ...@...
    //   .....
    //    ...
    //     .
    auto start = map.start();
    auto distance_matrix = distance_transform(map, start);
    uint64_t ans = 0;
    for (int y = 0; y < map.nrows; ++y) {
        for (int x = 0; x < map.ncols; ++x) {
            int dyx = distance_matrix.at(y,x);
            ans += dyx>=0 && dyx<=steps && dyx%2==steps%2;
        }
    }
    return ans;
}

uint64_t reachable_plots_cross_aux(uint64_t L, uint64_t S, uint64_t D, bool parity_match) {
    if (S+L < (L+1)/2+D) {
        return 0;
    }
    uint64_t N = (S + L - (L+1)/2 - D)/L;
    if (parity_match) {
        return N/2;
    }
    return (N+1)/2;
}

uint64_t reachable_plots_cross(const Map& map, int steps) {
    // parallel worlds in line with origin
    //     @
    //    .@.
    //   ..@..
    //  @@@.@@@
    //   ..@..
    //    .@.
    //     @
    
    const int L = map.nrows;
    const Vec2i start = map.start();
    uint64_t ans = 0;

    Matrix<int> dist_mat_bottom = distance_transform(map, Vec2i(L/2, L-1));
    Matrix<int> dist_mat_left = distance_transform(map, Vec2i(0, L/2));
    Matrix<int> dist_mat_up = distance_transform(map, Vec2i(L/2, 0));
    Matrix<int> dist_mat_right = distance_transform(map, Vec2i(L-1, L/2));
    for (int y = 0; y < L; ++y) {
        for (int x = 0; x < L; ++x) {
            bool parity_match = manhattan_distance(start, {x,y})%2 == steps%2;
            int d_bottom = dist_mat_bottom.at(y, x);
            int d_left   = dist_mat_left.at(y, x);
            int d_up     = dist_mat_up.at(y, x);
            int d_right  = dist_mat_right.at(y, x);
            if (d_bottom < 0) {
                continue;
            }
            ans += reachable_plots_cross_aux(L, steps, d_bottom, parity_match);
            ans += reachable_plots_cross_aux(L, steps, d_left,   parity_match);
            ans += reachable_plots_cross_aux(L, steps, d_up,     parity_match);
            ans += reachable_plots_cross_aux(L, steps, d_right,  parity_match);
        }
    }
    return ans;
}

uint64_t reachable_plots_quadrants_aux(uint64_t L, uint64_t S, uint64_t D, bool parity_match) {
    if (S+L < D+1) {
        return 0;
    }
    uint64_t N = (S+L-D-1)/L;
    if (parity_match) {
        return sum_of_odds(N/2);
    }
    if (N) {
        return sum_of_evens((N-1)/2);
    }
    return 0;
}

uint64_t reachable_plots_quadrants(const Map& map, int steps) {
    // parellel worlds inside diamond shape quadrants
    //     .
    //    @.@
    //   @@.@@
    //  .......
    //   @@.@@
    //    @.@
    //     .
    const int L = map.nrows;
    const Vec2i start = map.start();
    uint64_t ans = 0;

    Matrix<int> dist_mat_br = distance_transform(map, Vec2i(L-1, L-1));
    Matrix<int> dist_mat_ur = distance_transform(map, Vec2i(L-1, 0));
    Matrix<int> dist_mat_ul = distance_transform(map, Vec2i(0, 0));
    Matrix<int> dist_mat_bl = distance_transform(map, Vec2i(0, L-1));
    for (int y = 0; y < L; ++y) {
        for (int x = 0; x < L; ++x) {
            bool parity_match = manhattan_distance(start, {x,y})%2 == steps%2;
            int d_br = dist_mat_br.at(y, x);
            int d_ur = dist_mat_ur.at(y, x);
            int d_ul = dist_mat_ul.at(y, x);
            int d_bl = dist_mat_bl.at(y, x);
            if (d_br < 0) {
                continue;
            }
            ans += reachable_plots_quadrants_aux(L, steps, d_br, parity_match);
            ans += reachable_plots_quadrants_aux(L, steps, d_ur, parity_match);
            ans += reachable_plots_quadrants_aux(L, steps, d_ul, parity_match);
            ans += reachable_plots_quadrants_aux(L, steps, d_bl, parity_match);
        }
    }
    return ans;
}

uint64_t reachable_plots(const Map& map, int steps) {
    // the first three assumptions could be dropped and we could still get an
    // efficient solution, but it'd be more laborious.
    assert(map.nrows == map.ncols);
    assert(map.nrows%2 == 1);
    assert(map.start() == Vec2i(map.ncols/2,map.nrows/2));
    for (int y = 0; y < map.nrows; ++y) {
        assert(map.at(0,y) == '.');
        assert(y==map.nrows/2 || map.at(map.ncols/2,y) == '.');
        assert(map.at(map.ncols-1,y) == '.');
    }
    for (int x = 0; x < map.nrows; ++x) {
        assert(map.at(x,0) == '.');
        assert(x==map.ncols/2 || map.at(x,map.nrows/2) == '.');
        assert(map.at(x,map.nrows-1) == '.');
    }
    return reachable_plots_origin(map, steps) +
           reachable_plots_cross(map, steps) +
           reachable_plots_quadrants(map, steps);
}

int main() {
    Map map;
    map.read(cin);
    cout << reachable_plots(map, 64) << endl;
    cout << reachable_plots(map, 26501365) << endl;
}
