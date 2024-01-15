#include <iostream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

typedef vector<string> Map;


typedef pair<int,int> Point2i;


vector<pair<Point2i,int>> get_neighbors(const Map& map, int i, int j) {
    vector<pair<Point2i,int>> neighbors;
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



int risk_level(const Map& map) {
    int risk = 0;
    for (int i = 0; i < map.size(); ++i) {
        for (int j = 0; j < map[i].size(); ++j) {
            risk += is_low_point(map, i, j)*(map[i][j]+1 - '0');
        }
    }
    return risk;
}

int main() {
    Map map;
    
    string line;
    while (cin >> line) {
        map.push_back(line);
    }

    cout << risk_level(map) << endl;
}
