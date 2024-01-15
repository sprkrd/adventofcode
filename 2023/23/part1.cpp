#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

typedef array<int,2> Vec2i;

Vec2i operator+(const Vec2i& l, const Vec2i& r) {
    return Vec2i{l[0]+r[0],l[1]+r[1]};
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

    int ravel(const Vec2i& position) const {
        return position[1]*ncols + position[0];
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

    vector<Vec2i> get_neighbors(const Vec2i& position, int stride = 1) const {
        static constexpr Vec2i directions[] = {{0,1},{1,0},{0,-1},{-1,0}};
        vector<Vec2i> neighbors;
        switch (at(position)) {
            case '>':
                neighbors.push_back(position+Vec2i{stride,0});
                break;
            case 'v':
                neighbors.push_back(position+Vec2i{0,stride});
                break;
            case '<':
                neighbors.push_back(position+Vec2i{-stride,0});
                break;
            case '^':
                neighbors.push_back(position+Vec2i{0,-stride});
                break;
            default:
                for (const auto& dir : directions) {
                    Vec2i neighbor = position + dir;
                    if (isbetween(neighbor[0],0,ncols-1) &&
                        isbetween(neighbor[1],0,nrows-1) &&
                        at(neighbor) != '#') {
                        neighbors.push_back(neighbor);
                    }
                }
                break;
        } 
        return neighbors;
    }
};


template<class Collection, class ValueType>
int index(const Collection& collection, const ValueType& value) {
    auto it = find(collection.begin(), collection.end(), value);
    if (it == collection.end()) {
        return -1;
    }
    return it - collection.begin();
}


struct GraphSummary {
    vector<Vec2i> junctions;
    vector<vector<pair<int,int>>> adj_list;

    GraphSummary(const Map& map) {
        junctions.push_back({1, 0});
        for (int y = 1; y < map.nrows-1; ++y) {
            for (int x = 0; x < map.ncols; ++x) {
                Vec2i pos{x,y};
                if (map.at(pos) != '#') {
                    auto neighbors = map.get_neighbors(pos);
                    if (neighbors.size() > 2) {
                        junctions.push_back(pos);
                    }
                }
            }
        }
        junctions.push_back({map.ncols-2, map.nrows-1});
        adj_list.resize(junctions.size());
        for (int junction_idx = 0; junction_idx < (int)junctions.size(); ++junction_idx) {
            connect(map, junction_idx);
        }
    }

    void connect(const Map& map, int junction_idx) {
        vector<tuple<int,Vec2i,Vec2i>> stk{
            {0,junctions[junction_idx], junctions[junction_idx]}};
        while (!stk.empty()) {
            auto[dist,prev_cell,cell] = stk.back();
            stk.pop_back();
            int idx = index(junctions, cell);
            if (idx!=junction_idx && idx!=-1) {
                adj_list[junction_idx].emplace_back(dist, idx);
                continue;
            }
            for (auto&& neigh : map.get_neighbors(cell)) {
                if (neigh == prev_cell) {
                    continue;
                }
                stk.emplace_back(dist+1,cell,neigh);
            }
        }
    }

    void dot(ostream& out) const {
        out << "digraph G{\n";
        for (int node_idx = 0; node_idx < (int)junctions.size(); ++node_idx) {
            auto[x,y] = junctions[node_idx];
            out << 'n' << node_idx << " [label=\"(" << x << "," << y << ")\"]\n";
        }
        for (int from = 0; from < (int)junctions.size(); ++from) {
            for (const auto&[dist,to] : adj_list[from]) {
                out << 'n' << from << " -> " << 'n' << to
                    << " [label=" << dist << "]\n";
            }
        }
        out << "}\n";
    }
};

template<class Scalar>
struct Matrix {
    typedef typename vector<Scalar>::reference Reference;

    size_t nrows;
    size_t ncols;
    vector<Scalar> data;

    Matrix(size_t nrows, size_t ncols, Scalar default_val = Scalar()) :
        nrows(nrows), ncols(ncols), data(nrows*ncols, default_val) {}

    Scalar at(size_t i, size_t j) const {
        return data[i*ncols + j];
    }

    Reference at(size_t i, size_t j) {
        return data[i*ncols + j];
    }
};

template<class Collection, class ValueType>
bool contains(const Collection& collection, const ValueType& value) {
    return find(collection.begin(), collection.end(), value) != collection.end();
}

Matrix<bool> get_reachable_cells(const Map& map, const Vec2i& end) {
    Matrix<bool> reachable(map.nrows, map.ncols, false);
    vector<Vec2i> stk{end};
    while (!stk.empty()) {
        auto top = stk.back();
        stk.pop_back();
        if (reachable.at(top[1], top[0])) {
            continue;
        }
        reachable.at(top[1], top[0]) = true;
        for (auto&& neigh : map.get_neighbors(top, -1)) {
            stk.push_back(neigh);
        }
    }
    return reachable;
}

void get_longest_path(const Map& map,
                      vector<Vec2i>& longest_path,
                      vector<Vec2i>& current_path,
                      const Vec2i& end,
                      const Matrix<bool>& reachable) {
    if (current_path.back()==end) {
        if (current_path.size() > longest_path.size()) {
            longest_path = current_path;
        }
        return;
    }
    for (auto&& neighbor : map.get_neighbors(current_path.back())) {
        if (!contains(current_path, neighbor) &&
            reachable.at(neighbor[1], neighbor[0])) {
            current_path.push_back(neighbor);
            get_longest_path(map, longest_path, current_path, end, reachable);
            current_path.pop_back();
        }
    }
}

vector<Vec2i> get_longest_path(const Map& map) {
    Vec2i start = {1, 0};
    Vec2i end = {map.ncols-2, map.nrows-1};
    auto reachable = get_reachable_cells(map, end);
    vector<Vec2i> current_path{start};
    vector<Vec2i> longest_path;
    get_longest_path(map, longest_path, current_path, end, reachable);
    return longest_path;
}

int main() {
    Map map;
    map.read(cin);

    //for (const auto& neigh : map.get_neighbors({1,1})) {
    //    cout << neigh[0] << ' ' << neigh[1] << endl;
    //}
    
    auto longest_path = get_longest_path(map);
    //for (const auto& pos : longest_path) {
    //    map.at(pos) = 'O';
    //}

    //for (int y = 0; y < map.nrows; ++y) {
    //    for (int x = 0; x < map.ncols; ++x) {
    //        cout << map.at({x,y});
    //    }
    //    cout << endl;
    //}
    cout << (longest_path.size()-1) << endl;
}
