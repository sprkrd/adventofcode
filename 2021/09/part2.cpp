#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>
using namespace std;

typedef vector<string> Map;


typedef pair<int,int> Point2i;


vector<pair<Point2i,char>> get_neighbors(const Map& map, int i, int j) {
    vector<pair<Point2i,char>> neighbors;
    if (i > 0)
        neighbors.emplace_back(Point2i(i-1,j), map[i-1][j]);
    if (i < map.size()-1)
        neighbors.emplace_back(Point2i(i+1,j), map[i+1][j]);
    if (j > 0)
        neighbors.emplace_back(Point2i(i,j-1), map[i][j-1]);
    if (j < map[i].size()-1)
        neighbors.emplace_back(Point2i(i,j+1), map[i][j+1]);
    return neighbors;
}

bool is_low_point(const Map& map, int i, int j) {
    for (auto[_,height] : get_neighbors(map, i, j)) {
        if (height <= map[i][j])
            return false;
    }
    return true;
}

int basin_size(const Map& map, int i_low, int j_low) {
    vector<vector<bool>> visited(map.size(), vector<bool>(map[0].size()));
    queue<Point2i> open;
    int size = 0;
    open.emplace(i_low,j_low);
    while (!open.empty()) {
        auto[i,j] = open.front();
        open.pop();
        if (!visited[i][j]) {
            size += 1;
            visited[i][j] = true;
            for (auto[neigh,heigh] : get_neighbors(map, i, j)) {
                if (heigh != '9') {
                    open.push(neigh);
                }
            }
        }
    }
    return size;
}

int main() {
    Map map;
    
    string line;
    while (cin >> line) {
        map.push_back(line);
    }
    
    vector<int> basin_sizes;
    for (int i = 0; i < map.size(); ++i) {
        for (int j = 0; j < map[0].size(); ++j) {
            if (is_low_point(map, i, j)) {
                int size = basin_size(map, i, j);
                basin_sizes.push_back(size);
            }
        }
    }
    sort(basin_sizes.begin(), basin_sizes.end(), greater<int>());
    int ans = basin_sizes[0] * basin_sizes[1] * basin_sizes[2];
    cout << ans << endl;
}
