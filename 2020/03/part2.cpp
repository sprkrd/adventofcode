#include <iostream>
#include <string>
#include <vector>
using namespace std;

typedef vector<string> map_t;

int tree_count(const map_t& map, int vec_x, int vec_y) {
  int x = 0, y = 0, count = 0;
  int height = map.size();
  int width = map[0].size();
  while (y < height) {
    if (map[y][x] == '#')
      ++count;
    x += vec_x;
    y += vec_y;
    if (x >= width)
      x -= width;
  }
  return count;
}

int main() {
  const int vectors[][2] = { {1,1}, {3,1}, {5,1}, {7,1}, {1,2} };
  map_t map;
  string row;
  while (getline(cin,row))
    map.push_back(row);
  long long product = 1;
  for (auto[vec_x, vec_y] : vectors)
    product *= tree_count(map, vec_x, vec_y);
  cout << product << endl;
}
