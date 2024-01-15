#include <iostream>
#include <utility>
#include <vector>
using namespace std;

typedef pair<int,int> Vec2i;

enum class Direction { north, west, south, east};

bool is_right_turn(Direction dir, Direction next_dir) {
    int diff = (static_cast<int>(next_dir) - static_cast<int>(dir) + 4)%4;
    return diff == 3;
}

constexpr Direction all_directions[] = {
    Direction::north,
    Direction::west,
    Direction::south,
    Direction::east
};

Vec2i dir2vec(Direction dir) {
    static constexpr Vec2i vectors[] = {{0,-1}, {-1,0}, {0,1}, {1,0}};
    return vectors[static_cast<int>(dir)];
}

Vec2i operator+(const Vec2i& l, const Vec2i& r) {
    return Vec2i(l.first+r.first, l.second+r.second);
}

Vec2i operator*(int s, const Vec2i& v) {
    return Vec2i(s*v.first, s*v.second);
}

ostream& operator<<(ostream& out, const Vec2i& v) {
    return out << '(' << v.first << ", " << v.second << ')';
}

ostream& operator<<(ostream& out, Direction dir) {
    static constexpr const char* names[] = {"north", "west", "south", "east"};
    return out << names[static_cast<int>(dir)];
}

struct Edge {
    Direction move_dir;
    int stride;
};

int get_area(const vector<Edge>& polygon) {
    int right_turns = 0;
    int interior_area = 0;
    int curve_length = 0;
    Vec2i position(0,0);
    Direction previous_direction = polygon.back().move_dir;
    for (const auto& edge : polygon) {
        right_turns += is_right_turn(previous_direction, edge.move_dir);

        Vec2i next_position = position + edge.stride*dir2vec(edge.move_dir);
        int dx = next_position.first - position.first;
        int sum_y = next_position.second + position.second;
        interior_area += dx*sum_y;

        curve_length += edge.stride-1;

        position = next_position;
        previous_direction = edge.move_dir;
    }

    int left_turns = polygon.size() - right_turns;

    if (right_turns < left_turns) {
        swap(right_turns, left_turns);
    }

    int area = (2*abs(interior_area) +2*curve_length + 3*right_turns + left_turns)/4;
    return area;
}

vector<Edge> read_poly(istream& in) {
    vector<Edge> poly;
    char dir_c;
    int stride;
    string color;
    while (in >> dir_c >> stride >> color) {
        Edge edge;
        edge.stride = stride;
        edge.move_dir = dir_c=='U'? Direction::north :
            dir_c=='L'? Direction::west :
            dir_c=='D'? Direction::south :
            Direction::east;
        poly.push_back(edge);
    }
    return poly;
}

int main() {
    auto poly = read_poly(cin);
    cout << get_area(poly) << endl;
}
