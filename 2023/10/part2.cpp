#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <vector>
using namespace std;

#define DRAW_MAP

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

    int turn(int loc, int loc_prev, int loc_next) const {
        auto[x,y] = unravel(loc);
        auto[x_prev,y_prev] = unravel(loc_prev);
        auto[x_next,y_next] = unravel(loc_next);

        int dx_next = x_next - x;
        int dy_next = y_next - y;

        int dx_prev = x_prev - x;
        int dy_prev = y_prev - y;

        int z = dx_prev*dy_next - dy_prev*dx_next;
        return z==0? 0 : (z>0? 1 : -1);
    }

    vector<int> get_loop() const {
        vector<int> loop{start};
        int prev = start;
        int curr = connected_neighbors(start).front();
        while (curr != start) {
            loop.push_back(curr);
            for (int neigh : connected_neighbors(curr)) {
                if (neigh != prev) {
                    prev = curr;
                    curr = neigh;
                    break;
                }
            }
        }
        int turn_counter = 0;
        for (size_t i = 0; i < loop.size(); ++i) {
            int loc = loop[i];
            int loc_prev = loop[(i-1+loop.size())%loop.size()];
            int loc_next = loop[(i+1)%loop.size()];
            turn_counter += turn(loc, loc_prev, loc_next);
        }
        if (turn_counter > 0) {
            reverse(loop.begin()+1, loop.end());
        }
        return loop;
    }

    int loop_area() const {
        auto loop = get_loop();
        vector<bool> in_loop_contour(data.size());
        vector<int> stack;
        for (size_t i = 0; i < loop.size(); ++i) {
            int loc = loop[i];
            int loc_prev = loop[(i-1+loop.size())%loop.size()];
            int dloc = loc - loc_prev;
            if (dloc == 1) { // going east
                stack.push_back(loc+ncols);
                stack.push_back(loc_prev+ncols);
            }
            else if (dloc == -1) { // going west
                stack.push_back(loc-ncols);
                stack.push_back(loc_prev-ncols);
            }
            else if (dloc == ncols) { // going south
                stack.push_back(loc-1);
                stack.push_back(loc_prev-1);
            }
            else if (dloc == -ncols) { // going north
                stack.push_back(loc+1);
                stack.push_back(loc_prev+1);
            }

            in_loop_contour[loc] = true;
        }
        int area = 0;
        vector<bool> marked(data.size());
        while (!stack.empty()) {
            int loc = stack.back();
            stack.pop_back();
            if (marked[loc] || in_loop_contour[loc]) {
                continue;
            }
            marked[loc] = true;
            ++area;
            for (auto dir : all_directions) {
                int neigh = get_neighbor(loc, dir);
                stack.push_back(neigh);
            }
        }
#ifdef DRAW_MAP
        for (int y = 0; y < nrows; ++y) {
            for (int x = 0; x < ncols; ++x) {
                int loc = ravel(x,y);
                if (marked[loc]) {
                    cout << 'I';
                }
                else if (in_loop_contour[loc]) {
                    cout << 'L';
                }
                else {
                    cout << at(loc);
                }
            }
            cout << endl;
        }
#endif
        return area;
    }
};

int main() {
    Map map;
    map.read(cin);
    cout << map.loop_area() << endl;
}
