#include <cassert>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
using namespace std;

typedef pair<int,int> Point2i;
typedef pair<char,int> FoldInstruction;

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

int& get_coord(Point2i& point, char axis) {
    return axis == 'x'? get<0>(point) : get<1>(point);
}

int get_coord(const Point2i& point, char axis) {
    return axis == 'x'? get<0>(point) : get<1>(point);
}

set<Point2i> fold(const set<Point2i>& points,
                  const FoldInstruction& instruction) {
    auto[axis,fold_coord] = instruction;
    set<Point2i> result;
    for (const auto& point : points) {
        int point_coord = get_coord(point, axis);
        if (point_coord < fold_coord) {
            result.insert(point);
        }
        else if (point_coord > fold_coord) {
            Point2i transformed_point = point;
            get_coord(transformed_point, axis) = 2*fold_coord - point_coord;
            result.insert(transformed_point);
        }
        else {
            assert(false);
        }
    }
    return result;
}

int main() {
    set<Point2i> points;
    vector<FoldInstruction> instructions;

    string line;

    while (getline(cin, line) && !line.empty()) {
        auto parts = split(line, ",");
        points.emplace(stoi(parts[0]), stoi(parts[1]));
    }

    while (getline(cin, line)) {
        auto parts = split(line);
        parts = split(parts[2], "=");
        instructions.emplace_back(parts[0].front(), stoi(parts[1]));
    }

    points = fold(points, instructions[0]);

    cout << points.size() << endl;

    //for (auto[x,y] : points) {
        //cout << x << ',' << y << endl;
    //}
    //cout << endl;
    //for (auto[axis,coord] : instructions) {
        //cout << "fold along " << axis << '=' << coord << endl;
    //}
}
