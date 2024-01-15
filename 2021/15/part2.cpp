#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <utility>
#include <vector>
using namespace std;

typedef vector<string> Map;
typedef pair<int,int> Point2i;
typedef pair<int,Point2i> DistPoint;

constexpr int inf = numeric_limits<int>::max();

vector<DistPoint> get_neighbors(const Map& map, const Point2i& point) {
    vector<DistPoint> neighbors;
    auto[x,y] = point;
    if (x > 0)
        neighbors.emplace_back(map[y][x-1]-'0', Point2i(x-1,y));
    if (y > 0)
        neighbors.emplace_back(map[y-1][x]-'0', Point2i(x,y-1));
    if (x < map[0].size()-1)
        neighbors.emplace_back(map[y][x+1]-'0', Point2i(x+1,y));
    if (y < map.size()-1)
        neighbors.emplace_back(map[y+1][x]-'0', Point2i(x,y+1));
    return neighbors;
}

int shortest_path_cost(const Map& map, const Point2i& start, const Point2i& end) {
    priority_queue<DistPoint,vector<DistPoint>,greater<DistPoint>> open;
    vector<vector<int>> distance_from_start(map.size(), vector<int>(map[0].size(), inf));
    open.emplace(0, start);
    while (!open.empty()) {
        auto[acc_dist,point] = open.top();
        open.pop();
        if (point == end)
            return acc_dist;
        auto[x,y] = point;
        if (acc_dist < distance_from_start[y][x]) {
            distance_from_start[y][x] = acc_dist;
            for (auto[dist,neigh] : get_neighbors(map, point)) {
                open.emplace(acc_dist+dist, neigh);
            }
        }
    }
    return inf;
}

int main() {
    Map map;
    string line;
    while (getline(cin, line)) {
        map.push_back(line);
    }
    
    Map full_map(map.size()*5, string(map[0].size()*5, '1'));
    
    for (int y = 0; y < full_map.size(); ++y) {
        for (int x = 0; x < full_map[0].size(); ++x) {
            int x_orig = x % map[0].size();
            int y_orig = y % map.size();
            int tile_x = x / map[0].size();
            int tile_y = y / map.size();
            int inc = tile_x + tile_y;
            full_map[y][x] = '1' + (int)(map[y_orig][x_orig]-'0' -1+inc)%9;
        }
    }
    
    cout << shortest_path_cost(full_map, Point2i(0,0),
                                Point2i(full_map[0].size()-1, full_map.size()-1)) << endl;
}
