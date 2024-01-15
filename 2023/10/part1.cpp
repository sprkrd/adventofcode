#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <vector>
using namespace std;

enum class Direction { north, east, south, west };
Direction all_directions[] = { Direction::north, Direction::east, Direction::south, Direction::west };

inline pair<int,int> dir2vec(Direction dir) {
    static constexpr pair<int,int> veclu[] = {{0,-1}, {1,0}, {0,1}, {-1,0}};
    return veclu[static_cast<int>(dir)];
}

template<class Container, class Item = typename Container::value_type>
bool contains(const Container& container, Item&& item) {
    return find(container.begin(), container.end(), forward<Item>(item)) != container.end();
}

bool isbetween(int x, int lo, int hi) {
    return lo<=x && x<=hi;
}

struct Map {
    int start;
    int nrows;
    int ncols;
    string data;

    void read(istream& in) {
        nrows = 0;
        ncols = 0;
        data.clear();
        string line;
        while (getline(in, line)) {
            ncols = line.size();
            ++nrows;
            data += line;
        }
        start = data.find_first_of('S');

    }

    pair<int,int> unravel(int loc) const {
        return {loc%ncols, loc/ncols};
    }

    int ravel(int x, int y) const {
        return y*ncols + x;
    }

    char at(int loc) const {
        return data[loc];
    }

    char at(int x, int y) const {
        return at(ravel(x,y));
    }

    int get_neighbor(int loc, Direction dir) const {
        auto[x,y] = unravel(loc);
        auto[dx,dy] = dir2vec(dir);
        int nx = x+dx;
        int ny = y+dy;
        if (isbetween(nx,0,ncols-1) && isbetween(ny,0,nrows-1)) {
            return ravel(nx,ny);
        }
        return -1;
    }

    vector<int> connected_neighbors(int loc) const {
        vector<int> neighbors;
        switch (data[loc]) {
            case '|':
                neighbors.push_back(get_neighbor(loc, Direction::north));
                neighbors.push_back(get_neighbor(loc, Direction::south));
                break;
            case '-':
                neighbors.push_back(get_neighbor(loc, Direction::west));
                neighbors.push_back(get_neighbor(loc, Direction::east));
                break;
            case 'L':
                neighbors.push_back(get_neighbor(loc, Direction::north));
                neighbors.push_back(get_neighbor(loc, Direction::east));
                break;
            case 'J':
                neighbors.push_back(get_neighbor(loc, Direction::north));
                neighbors.push_back(get_neighbor(loc, Direction::west));
                break;
            case '7':
                neighbors.push_back(get_neighbor(loc, Direction::west));
                neighbors.push_back(get_neighbor(loc, Direction::south));
                break;
            case 'F':
                neighbors.push_back(get_neighbor(loc, Direction::east));
                neighbors.push_back(get_neighbor(loc, Direction::south));
                break;
            case 'S':
                for (auto dir : all_directions) {
                    int neigh = get_neighbor(loc, dir);
                    if (contains(connected_neighbors(neigh), loc)) {
                        neighbors.push_back(neigh);
                    }
                }
                break;
            case '.':
                break;
            default:
                break;

        }
        return neighbors;
    }
};

int furthest_distance_from_start(const Map& map) {
    queue<pair<int,int>> open;
    open.emplace(0, map.start);
    vector<int> distance(map.data.size(), -1);
    int largest_dist = 0;
    while (!open.empty()) {
        auto[dist_loc, loc] = open.front();
        open.pop();
        if (distance[loc] >= 0) {
            continue;
        }
        distance[loc] = dist_loc;
        largest_dist = max(largest_dist, dist_loc);
        for (int neigh : map.connected_neighbors(loc)) {
            open.emplace(dist_loc+1, neigh);
        }
    }
    return largest_dist;
}

int main() {
    Map map;
    map.read(cin);
    cout << furthest_distance_from_start(map) << endl;
}
