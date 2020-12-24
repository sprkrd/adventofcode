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

int main() {
  set<vec2i> black_tiles;
  string line;
  while (getline(cin,line)) {
    auto directions = parse_directions(line);
    auto tile = sum(directions);
    flip(black_tiles, tile);
  }
  cout << black_tiles.size() << endl;
}

