#include <iostream>
#include <string>
#include <set>
#include <tuple>
#include <vector>
using namespace std;

typedef tuple<int,int,int> point_t;

typedef set<point_t> point_cloud_t;

vector<point_t> get_neighbors(const point_cloud_t& points, const point_t& point, bool active = true) {
  auto[x0,y0,z0] = point;
  vector<point_t> neighbors;
  neighbors.reserve(26);
  for (int x = x0-1; x <= x0+1; ++x) {
    for (int y = y0-1; y <= y0+1; ++y) {
      for (int z = z0-1; z <= z0+1; ++z) {
        bool include = ( (x!=x0) || (y!=y0) || (z!=z0) ) &&
          ( active == (points.find({x,y,z}) != points.end()) );
        if (include)
          neighbors.emplace_back(x,y,z);
      }
    }
  }
  return neighbors;
}

point_cloud_t calculate_frontier(const point_cloud_t& points) {
  point_cloud_t frontier;
  for (const auto& point : points) {
    auto inactive_neighbors = get_neighbors(points, point, false);
    frontier.insert(inactive_neighbors.begin(), inactive_neighbors.end());
  }
  return frontier;
}

void update(point_cloud_t& points) {
  point_cloud_t updated;
  point_cloud_t frontier = calculate_frontier(points);
  for (const auto& point : points) {
    int active_neighbor_count = get_neighbors(points, point).size();
    if (active_neighbor_count == 2 || active_neighbor_count == 3)
      updated.insert(point);
  }
  for (const auto& point : frontier) {
    int active_neighbor_count = get_neighbors(points, point).size();
    if (active_neighbor_count == 3)
      updated.insert(point);
  }
  updated.swap(points);
}

int main() {
  point_cloud_t points;
  int row = 0;
  string line;
  while (getline(cin,line)) {
    for (int col = 0; col < line.length(); ++col) {
      if (line[col] == '#')
        points.emplace(row,col,0);
    }
    ++row;
  }
  for (int i = 0; i < 6; ++i) {
    cout << i << ' ' << points.size() << endl;
    update(points);
  }
  cout << points.size() << endl;
}

