#include <iostream>
#include <vector>
#include <string>
using namespace std;

typedef vector<string> map_t;

int count_adjacent_occupied(const map_t& map, int x, int y) {
  int width = map[0].size();
  int height = map.size();
  int adjacent_occupied = 0;
  for (int y_adj = max(y-1,0); y_adj <= min(y+1,height-1); ++y_adj) {
    for (int x_adj = max(x-1,0); x_adj <= min(x+1,width-1); ++x_adj)
      adjacent_occupied += (x!=x_adj||y!=y_adj) * (map[y_adj][x_adj]=='#');
  }
  return adjacent_occupied;
}

int update(map_t& map) {
  map_t copy = map;
  int number_of_changes = 0;
  int width = map[0].size();
  int height = map.size();
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int adj_occupied = count_adjacent_occupied(map,x,y);
      if (map[y][x] == 'L' && !adj_occupied) {
        copy[y][x] = '#';
        ++number_of_changes;
      }
      else if (map[y][x] == '#' && adj_occupied >= 4) {
        copy[y][x] = 'L';
        ++number_of_changes;
      }
    }
  }
  map.swap(copy);
  return number_of_changes;
}

int number_of_occupied_seats(const map_t& map) {
  int number_of_occupied = 0;
  int width = map[0].size();
  int height = map.size();
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x)
      number_of_occupied += map[y][x] == '#';
  }
  return number_of_occupied;
}

ostream& operator<<(ostream& out, const map_t& map) {
  for (const auto& row : map)
    out << row << endl;
  return out;
}

int main() {
  map_t map;
  string row;
  while (cin >> row)
    map.push_back(move(row));
  while (update(map));
  cout << map << endl;
  cout << number_of_occupied_seats(map) << endl;
}

