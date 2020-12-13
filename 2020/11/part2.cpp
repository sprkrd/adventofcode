#include <iostream>
#include <vector>
#include <string>
using namespace std;

typedef vector<string> map_t;

bool between(int x, int lo, int hi) {
  return lo <= x && x <= hi;
}

bool next_seat_occupied(const map_t& map, int x, int y, int vec_x, int vec_y) {
  while (between(x+=vec_x,0,map[0].size()-1) && between(y+=vec_y,0,map.size()-1))
    if (map[y][x] != '.') return map[y][x] == '#';
  return false;
}

int count_adjacent_occupied(const map_t& map, int x, int y) {
  int adjacent_occupied = next_seat_occupied(map,x,y,1,0) +
                          next_seat_occupied(map,x,y,1,1) +
                          next_seat_occupied(map,x,y,0,1) +
                          next_seat_occupied(map,x,y,-1,1) +
                          next_seat_occupied(map,x,y,-1,0) +
                          next_seat_occupied(map,x,y,-1,-1) +
                          next_seat_occupied(map,x,y,0,-1) +
                          next_seat_occupied(map,x,y,1,-1);
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
      else if (map[y][x] == '#' && adj_occupied >= 5) {
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

