#include <iostream>
#include <stack>
#include <unordered_set>
#include "utils.hpp"
using namespace std;

struct Cube {
    int x;
    int y;
    int z;

    bool operator==(const Cube& other) const {
        return x == other.x &&
               y == other.y &&
               z == other.z;
    }

    array<Cube,6> neighbors() const {
        array<Cube,6> result;
        result[0] = Cube{x+1,y,z};
        result[1] = Cube{x-1,y,z};
        result[2] = Cube{x,y+1,z};
        result[3] = Cube{x,y-1,z};
        result[4] = Cube{x,y,z+1};
        result[5] = Cube{x,y,z-1};
        return result;
    }

    template <size_t index>
    int get() const {
        static_assert(0 <= index && index <= 2);
        return *(&x + index);
    }
};

namespace std {
template <>
struct hash<Cube> {
    size_t operator()(const Cube& cube) const noexcept {
        size_t seed = 0;
        ash::hash_combine(seed, cube.x);
        ash::hash_combine(seed, cube.y);
        ash::hash_combine(seed, cube.z);
        return seed;
    }
};
}

unordered_set<Cube> read_input(istream& in) {
    unordered_set<Cube> cubes;
    string line;
    while (getline(cin,line)) {
        auto ints = ash::every_int(line);
        cubes.insert(Cube{ints.at(0), ints.at(1), ints.at(2)});
    }
    return cubes;
}

struct Bounding_box {
    int x_min;
    int x_max;
    int y_min;
    int y_max;
    int z_min;
    int z_max;

    Bounding_box(const unordered_set<Cube>& cubes) {
        const int max_int = numeric_limits<int>::max();
        const int min_int = numeric_limits<int>::min();
        x_min = y_min = z_min = max_int;
        x_max = y_max = z_max = min_int;
        for (auto[x,y,z] : cubes) {
            x_min = min(x,x_min);
            x_max = max(x,x_max);
            y_min = min(y,y_min);
            y_max = max(y,y_max);
            z_min = min(z,z_min);
            z_max = max(z,z_max);
        }
    }

    bool contains(const Cube& cube) const {
        return x_min <= cube.x && cube.x <= x_max &&
               y_min <= cube.y && cube.y <= y_max &&
               z_min <= cube.z && cube.z <= z_max;
    }
};

bool is_trapped(const unordered_set<Cube>& cubes, const Bounding_box& bb, const Cube& seed,
                unordered_set<Cube>& trapped_air, unordered_set<Cube>& free_air) {
    if (trapped_air.count(seed)) {
        return true;
    }
    if (free_air.count(seed)) {
        return false;
    }
    bool answer = true;
    unordered_set<Cube> visited;
    stack<Cube> stk;
    stk.push(seed);
    while (!stk.empty()) {
        Cube cube = stk.top();
        stk.pop();
        if (!bb.contains(cube)) {
            answer = false;
            continue;
        }
        if (visited.count(cube)) {
            continue;
        }
        visited.insert(cube);
        for (auto&& neigh : cube.neighbors()) {
            if (!cubes.count(neigh)) {
                stk.push(neigh);
            }
        }
    }
    if (answer) {
        trapped_air.insert(visited.begin(), visited.end());
    }
    else {
        free_air.insert(visited.begin(), visited.end());
    }
    return answer;
}

int external_surface(const unordered_set<Cube>& cubes) {
    Bounding_box bb(cubes);
    unordered_set<Cube> trapped_air;
    unordered_set<Cube> free_air;
    int result = 0;
    for (auto&& cube : cubes) {
        for (auto&& neigh : cube.neighbors()) {
            if (!cubes.count(neigh)) {
                result += !is_trapped(cubes, bb, neigh, trapped_air, free_air);
            }
        }
    }
    return result;
}

int main() {
    auto cubes = read_input(cin);
    cout << external_surface(cubes) << endl;
}

