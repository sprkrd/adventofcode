#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

enum class Direction { north, east, south, west };

void tilt(vector<string>& platform, Direction dir) {
    int nrows = platform.size();
    int ncols = platform[0].size();

    int begin, end, inc;
    bool horizontal;
    switch (dir) {
        case Direction::north:
            begin = 0;
            end = nrows;
            inc = 1;
            horizontal = false;
            break;
        case Direction::east:
            begin = ncols - 1;
            end = -1;
            inc = -1;
            horizontal = true;
            break;
        case Direction::south:
            begin = nrows - 1;
            end = -1;
            inc = -1;
            horizontal = false;
            break;
        case Direction::west:
            begin = 0;
            end = ncols;
            inc = 1;
            horizontal = true;
            break;
        default:
            assert(false);
    }

    if (horizontal) {
        for (int y = 0; y < nrows; ++y) {
            int free_x = -1;
            for (int x = begin; x != end; x += inc) {
                if (platform[y][x]=='#') {
                    free_x = -1;
                }
                else if (platform[y][x]=='.' && free_x==-1) {
                    free_x = x;
                }
                else if (platform[y][x]=='O' && free_x!=-1) {
                    platform[y][free_x] = 'O';
                    platform[y][x] = '.';
                    free_x += inc;
                }
            }
        }
    }
    else {
        for (int x = 0; x < ncols; ++x) {
            int free_y = -1;
            for (int y = begin; y != end; y += inc) {
                if (platform[y][x]=='#') {
                    free_y = -1;
                }
                else if (platform[y][x]=='.' && free_y==-1) {
                    free_y = y;
                }
                else if (platform[y][x]=='O' && free_y!=-1) {
                    platform[free_y][x] = 'O';
                    platform[y][x] = '.';
                    free_y += inc;
                }
            }
        }
    }
}

vector<string> spin_cycle(const vector<string>& platform) {
    auto successor = platform;
    tilt(successor, Direction::north);
    tilt(successor, Direction::west);
    tilt(successor, Direction::south);
    tilt(successor, Direction::east);
    return successor;
}

template<class F, class S>
pair<size_t,size_t> find_cycle(const F& fun, const S& state0) {
    S tortoise = fun(state0);
    S hare = fun(tortoise);
    while (tortoise != hare) {
        tortoise = fun(tortoise);
        hare = fun(fun(hare));
    }
    hare = state0;
    size_t mu = 0;
    while (tortoise != hare) {
        hare = fun(hare);
        ++mu;
    }
    hare = fun(tortoise);
    size_t lam = 1;
    while (tortoise != hare) {
        hare = fun(hare);
        ++lam;
    }
    return {mu,lam};

}

ostream& operator<<(ostream& out, const vector<string>& platform) {
    for (const auto& row : platform) {
        out << row << '\n';
    }
    return out;
}

size_t calculate_load(const vector<string>& platform) {
    size_t load = 0;
    size_t weight = platform.size();
    for (const auto& row : platform) {
        load += weight*count(row.begin(), row.end(), 'O');
        --weight;
    }
    return load;
}

int main() {
    constexpr size_t num_spin_cycles = 1'000'000'000;
    vector<string> platform;
    string line;
    while (getline(cin, line)) {
        platform.push_back(move(line));
    }
    // cout << platform << endl;
    // tilt(platform, Direction::north);
    // cout << platform << endl;
    //
    auto[mu,lam] = find_cycle(&spin_cycle, platform);
    for (size_t i = 0; i < min(num_spin_cycles, mu); ++i) {
        platform = spin_cycle(platform);
    }
    if (num_spin_cycles > mu) {
        size_t extra_spins = (num_spin_cycles - mu)%lam;
        for (size_t i = 0; i < extra_spins; ++i) {
            platform = spin_cycle(platform);
        }
    }
    cout << calculate_load(platform) << endl;
}
