#include <iostream>
#include <string>
#include <vector>
using namespace std;

typedef vector<string> map_t;

int main() {
  const int vec_x = 3;
  const int vec_y = 1;

  map_t map;
  string row;
  while (getline(cin,row))
    map.push_back(row);
  unsigned x = 0, y = 0, tree_count = 0;
  while (y < map.size()) {
    if (map[y][x] == '#')
      ++tree_count;
    x += vec_x;
    y += vec_y;
    if (x >= map[0].size())
      x -= map[0].size();
  }
  cout << tree_count << endl;
}
