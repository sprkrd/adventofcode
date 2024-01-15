#include <cassert>
#include <iostream>
#include <set>
#include <string>
#include <tuple>
#include <vector>
using namespace std;

typedef tuple<int,int> vec2i;

ostream& operator<<(ostream& out, const vec2i& vec) {
  return out << '(' << get<0>(vec) << ',' << get<1>(vec) << ')';
}

template<class T, class Alloc>
ostream& operator<<(ostream& out, const vector<T,Alloc>& vec) {
  out << '[';
  for (auto it = vec.begin(); it != vec.end(); ++it) {
    if (it != vec.begin())
      out << ", ";
    out << *it;
  }
  return out << ']';
}

vector<vec2i> parse_directions(const string& line) {
  vector<vec2i> directions;
  for (int i = 0; i < line.size(); ++i) {
    int x = 0, y = 0;
    if (line[i]=='n') {
      y = 1;
      x = line[++i]=='e';
    }
    else if (line[i]=='s') {
      y = -1;
      x = -(line[++i]=='w');
    }
    else 
      x = (line[i]=='e') - (line[i]=='w');
    directions.emplace_back(x,y);
  }
  return directions;
}

vec2i sum(const vector<vec2i>& directions) {
  int x_dst = 0, y_dst = 0;
  for (auto [x,y] : directions) {
    x_dst += x;
    y_dst += y;
  }
  return make_tuple(x_dst,y_dst);
}

void flip(set<vec2i>& black_tiles, const vec2i& tile) {
  auto it = black_tiles.find(tile);
  if (it != black_tiles.end()) black_tiles.erase(it);
  else black_tiles.insert(tile);
}

vector<vec2i> get_white_neighbors(const set<vec2i>& tiles,
                                  const vec2i& tile) {
  static const vec2i neighbor_vecs[] ={
    {-1,0},{1,0},{0,1},{1,1},{0,-1},{-1,-1}};
  vector<vec2i> empty;
  empty.reserve(6);
  auto[x,y] = tile;
  for (auto[vec_x,vec_y] : neighbor_vecs) {
    vec2i neighbor(x+vec_x,y+vec_y);
    if (!tiles.count(neighbor))
      empty.push_back(neighbor);
  }
  return empty;
}

set<vec2i> frontier(const set<vec2i>& tiles) {
  set<vec2i> result;
  for (const auto& tile : tiles) {
    auto neighbors = get_white_neighbors(tiles, tile);
    result.insert(neighbors.begin(), neighbors.end());
  }
  return result;
}

void update(set<vec2i>& black_tiles) {
  set<vec2i> result;
  auto white_tiles = frontier(black_tiles);
  for (const auto& tile : black_tiles) {
    int black_neighbors = 6 - get_white_neighbors(black_tiles,tile).size();
    if (black_neighbors == 1 || black_neighbors == 2)
      result.insert(tile);
  }
  for (const auto& tile : white_tiles) {
    int black_neighbors = 6 - get_white_neighbors(black_tiles,tile).size();
    if (black_neighbors == 2)
      result.insert(tile);
  }
  result.swap(black_tiles);
}

int main() {
  set<vec2i> black_tiles;
  string line;
  while (getline(cin,line)) {
    auto directions = parse_directions(line);
    auto tile = sum(directions);
    flip(black_tiles, tile);
  }
  cout << "Day 0: " << black_tiles.size() << endl;
  for (int day = 1; day <= 100; ++day) {
    update(black_tiles);
    cout << "Day " << day << ": " << black_tiles.size() << endl;
  }
}

