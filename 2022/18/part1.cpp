#include <iostream>
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

int surface(const unordered_set<Cube>& cubes) {
    int total_surface = 0;
    for (const Cube& cube : cubes) {
        for (auto&& neigh : cube.neighbors()) {
            total_surface += !cubes.count(neigh);
        }
    }
    return total_surface;
}

int main() {
    auto cubes = read_input(cin);
    cout << surface(cubes) << endl;
}
