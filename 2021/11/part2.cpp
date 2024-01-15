#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>
using namespace std;

typedef pair<int,int> Point2i;
typedef vector<int> Row;
typedef vector<Row> Map;

vector<Point2i> get_neighbors(const Map& map, int i, int j) {
    int width = map[0].size();
    int height = map.size();

    vector<Point2i> neighbors;
    if (i > 0) {
        if (j > 0)
            neighbors.emplace_back(i-1, j-1);
        neighbors.emplace_back(i-1, j);
        if (j < width-1)
            neighbors.emplace_back(i-1, j+1);
    }

    if (j > 0)
        neighbors.emplace_back(i, j-1);
    if (j < width-1)
        neighbors.emplace_back(i, j+1);

    if (i < height-1) {
        if (j > 0)
            neighbors.emplace_back(i+1, j-1);
        neighbors.emplace_back(i+1, j);
        if (j < width-1)
            neighbors.emplace_back(i+1, j+1);
    }
    
    return neighbors;
}

int step(Map& map) {
    queue<Point2i> scheduled_flashes;

    for (int i = 0; i < map.size(); ++i) {
        for (int j = 0; j < map[i].size(); ++j) {
            if (++map[i][j] > 9) {
                scheduled_flashes.emplace(i,j);
            }
        }
    }

    int num_flashes = 0;
    while (!scheduled_flashes.empty()) {
        auto[i,j] = scheduled_flashes.front();
        scheduled_flashes.pop();

        if (map[i][j] != 0) {
            ++num_flashes;
            map[i][j] = 0;
            for (auto[i_neigh,j_neigh] : get_neighbors(map, i, j)) {
                if (map[i_neigh][j_neigh] != 0 && ++map[i_neigh][j_neigh] > 9) {
                    scheduled_flashes.emplace(i_neigh, j_neigh);
                }
            }
        }
    }
    return num_flashes;
}

void show_map(const Map& map) {
    for (const auto& row : map) {
        for (int cell : row) {
            cout << cell;
        }
        cout << endl;
    }
}

int main() {
    Map map;

    string line;
    while (getline(cin, line)) {
        Row row(line.length());
        for (int i = 0; i < line.size(); ++i) {
            row[i] = line[i] - '0';
        }
        map.push_back(move(row));
    }
    int n_octopuses = map.size() * map[0].size();
    int i = 1;
    while (step(map) != n_octopuses) {
        ++i;
    }
    cout << i << endl;
}
