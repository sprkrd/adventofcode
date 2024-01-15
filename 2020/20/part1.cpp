#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <tuple>
using namespace std;

struct tile_t {
  int id;
  vector<string> data;
  string get_column(int j) const {
    string column(data.size(),0);
    for (int i = 0; i < data.size(); ++i)
      column[i] = data[i][j];
    return column;
  }
  bool is_lr_compatible(const tile_t& other) const {
    return get_column(data[0].size()-1) == other.get_column(0);
  }
  bool is_td_compatible(const tile_t& other) const {
    return data.back() == other.data[0];
  }
};

ostream& operator<<(ostream& out, const tile_t& tile) {
  out << "Tile " << tile.id << ':';
  for (const auto& row : tile.data)
    out << '\n' << row;
  return out;
}

vector<string> rotate(const vector<string>& mat) {
  int width = mat[0].size();
  int height = mat.size();
  vector<string> rotated(width,string(height,0));
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j)
      rotated[i][j] = mat[j][width-1-i];
  }
  return rotated;
}

vector<string> flipx(vector<string> mat) {
  int width = mat[0].size();
  int height = mat.size();
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width/2; ++j)
      swap(mat[i][j], mat[i][width-1-j]);
  }
  return mat;
}

vector<tile_t> parse_tiles(istream& in) {
  vector<tile_t> tiles;
  string line;
  while (getline(cin,line)) {
    tiles.emplace_back();
    tiles.back().id = stoi(line.substr(line.length()-5,4));
    while (getline(cin,line) && !line.empty())
      tiles.back().data.push_back(line);
  }
  return tiles;
}

typedef tuple<int,int> transform_id;

ostream& operator<<(ostream& out, const transform_id& trans_id) {
  return out << get<0>(trans_id) << ' ' << get<1>(trans_id);
}

void intersection(vector<transform_id>& l, const vector<transform_id>& r) {
  vector<transform_id> inter;
  set_intersection(l.begin(),l.end(),r.begin(),r.end(),back_inserter(inter));
  inter.swap(l);
}

struct csp_t {
  map<transform_id,tile_t> transformed_tiles;
  map<transform_id,vector<transform_id>> td_compatible, lr_compatible;
  set<int> remaining_tiles;
  vector<vector<transform_id>> solution;

  vector<transform_id> var_domain(int i, int j) const {
    vector<transform_id> domain;
    if (i > 0) {
      domain = td_compatible.find(solution[i-1][j])->second;
      if (j > 0)
        intersection(domain, lr_compatible.find(solution[i][j-1])->second);
    }
    else if (j > 0)
      domain = lr_compatible.find(solution[i][j-1])->second;
    else {
      for (int tile_id : remaining_tiles) {
        for (int i = 0; i < 8; ++i)
          domain.emplace_back(tile_id,i);
      }
    }
    return domain;
  }
};

csp_t create_csp(const vector<tile_t>& tiles) {
  csp_t csp;
  int square_side = round(sqrt(tiles.size()));
  csp.solution = vector<vector<transform_id>>(square_side,vector<transform_id>(square_side));
  for (auto tile : tiles) {
    csp.remaining_tiles.insert(tile.id);
    // rotations
    for (int i = 0; i < 4; ++i) {
      csp.transformed_tiles.emplace(transform_id{tile.id,i},tile);
      tile.data = rotate(tile.data);
    }
    // flip+rotation
    tile.data = flipx(tile.data);
    for (int i = 4; i < 8; ++i) {
      csp.transformed_tiles.emplace(transform_id{tile.id,i},tile);
      tile.data = rotate(tile.data);
    }
  }
  for (const auto&[trans_id1,tile1] : csp.transformed_tiles) {
    vector<transform_id> td_compatible, lr_compatible;
    for (const auto&[trans_id2,tile2] : csp.transformed_tiles) {
      if (get<0>(trans_id1) != get<0>(trans_id2)) {
        if (tile1.is_td_compatible(tile2))
          td_compatible.push_back(trans_id2);
        if (tile1.is_lr_compatible(tile2))
          lr_compatible.push_back(trans_id2);
      }
    }
    csp.td_compatible.emplace(trans_id1, move(td_compatible));
    csp.lr_compatible.emplace(trans_id1, move(lr_compatible));
  }
  return csp;
}

bool solve(csp_t& csp, int i = 0, int j = 0) {
  if (csp.remaining_tiles.empty())
    return true;
  int square_side = csp.solution.size();
  int j_next = j+1<square_side? j+1 : 0;
  int i_next = i + !j_next;
  vector<transform_id> domain = csp.var_domain(i,j);
  for (const auto& trans_id : domain) {
    csp.remaining_tiles.erase(get<0>(trans_id));
    csp.solution[i][j] = trans_id;
    if (solve(csp,i_next,j_next))
      return true;
    csp.remaining_tiles.insert(get<0>(trans_id));
  }
  return false;
}

int main() {
  auto tiles = parse_tiles(cin);

  auto csp = create_csp(tiles);
  //for (const auto&[trans_id,tile] : csp.transformed_tiles) {
    //auto[tile_id,mod_id] = trans_id;
    //cout << '(' << mod_id << ") " << tile << "\n\n";
  //}

  //for (const auto&[trans_id,compatible] : csp.td_compatible) {
    //cout << trans_id << ": ";
    //for (int i = 0; i < compatible.size(); ++i) {
      //if (i > 0) cout << ", ";
      //cout << compatible[i];
    //}
    //cout << '\n';
  //}
  assert(solve(csp));
  for (const auto& row : csp.solution) {
    for (auto[tile_id,_] : row)
      cout << tile_id << ' ';
    cout << endl;
  }

  long long product = (long long)(get<0>(csp.solution[0][0])) *
                      get<0>(csp.solution.back()[0]) *
                      get<0>(csp.solution[0].back()) *
                      get<0>(csp.solution.back().back());
  cout << product << endl;
}

