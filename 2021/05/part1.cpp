#include <cassert>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

typedef pair<int,int> Point2i;

typedef pair<Point2i,Point2i> Segment;

typedef vector<int> Row;
typedef vector<Row> Map;

vector<string> split(const string& str, const string& delim=" \n\t") {
  vector<string> parts;
  size_t pos = 0;
  while (pos < str.length()) {
    size_t pos_next = str.find_first_of(delim, pos);
    if (pos_next > pos)
      parts.push_back(str.substr(pos, pos_next-pos));
    pos = pos_next == string::npos? string::npos : pos_next + 1;
  }
  return parts;
}

int sign(int x) {
    if (x > 0) {
        return 1;
    }
    else if (x < 0) {
        return -1;
    }
    return 0;
}

void draw(Map& map, const Segment& segment) {
    auto[s1,s2] = segment;
    auto[x1,y1] = s1;
    auto[x2,y2] = s2;
    int dx = x2 - x1;
    int dy = y2 - y1;
    assert(dx == 0 || dy == 0 || abs(dx) == abs(dy));
    dx = sign(dx);
    dy = sign(dy);
    int x = x1;
    int y = y1;
    map[y][x] += 1;
    while (x != x2 || y != y2) {
        x += dx;
        y += dy;
        map[y][x] += 1;
    }
}

int main() {
    string line;
    vector<Segment> segments;
    int maxx, maxy;
    maxx = maxy = 0;
    while (getline(cin, line)) {
        auto parts = split(line);
        assert(parts[1] == "->");
        auto lhs = split(parts[0], ",");
        auto rhs = split(parts[2], ",");
        int x1 = stoi(lhs[0]);
        int y1 = stoi(lhs[1]);
        int x2 = stoi(rhs[0]);
        int y2 = stoi(rhs[1]);
        if (x1 == x2 || y1 == y2) { // only horizontal and vertical lines
            segments.emplace_back(Point2i{x1,y1},
                                  Point2i{x2,y2});
            maxx = max(maxx, max(x1,x2));
            maxy = max(maxy, max(y1,y2));
        }
    }

    Map map(maxy+1, Row(maxx+1));
    for (const auto& segment : segments) {
        draw(map, segment);
    }

    //for (const auto& row : map) {
        //for (int cell : row) {
            //cout << cell;
        //}
        //cout << endl;
    //}

    int count = 0;
    for (const auto& row : map) {
        for (int cell : row) {
            count += cell > 1;
        }
    }
    cout << count << endl;
}
