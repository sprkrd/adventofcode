#include <iostream>
#include <string>
#include <tuple>
#include <unordered_set>
using namespace std;

typedef tuple<int,int,int> point_t;

struct point_hasher {
  size_t operator()(const point_t& point) const {
    size_t x,y,z;
    tie(x,y,z) = point;
    return (x<<40) + (y<<20) + z;
  }
};

typedef unordered_set<point_t, point_hasher> point_cloud_t;

int count_neighbors(const point_cloud_t& points, const point_t& point) {
  int count = 0;
  auto[x0,y0,z0] = point;
  for (int x = x0-1; x <= x0+1; ++x) {
    for (int y = y0-1; y <= y0+1; ++y) {
      for (int z = z0-1; z <= z0+1; ++z)
        count += points.count({x,y,z});
    }
  }
  return count-1; // subtract 1 so point doesn't get counted as a neighbor
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
  for (auto[x,y,z] : points)
    cout << x << ' ' << y << ' ' << z << endl;
}

