#include <iostream>
#include <queue>
#include <string>
#include <vector>
using namespace std;

typedef pair<int,int> Point2i;

struct Map {
    string data;
    int width, height;

    Map() {}

    Map(int width, int height, int default_value = 0) :
        data(width*height, '0'+default_value),
        width(width),
        height(height) {}

    void read(istream& in) {
        getline(in, data);
        width = data.length();
        height = 1;
        string line;
        while (getline(in, line)) {
            ++height;
            data.insert(data.end(), line.begin(), line.end());
        }
    }

    char operator()(int i, int j) const {
        return data[i*width+j];
    }

    char& operator()(int i, int j) {
        return data[i*width+j];
    }

    char position_height(int i, int j) const {
        char position_height = (*this)(i,j);
        if (position_height == 'E')
            position_height = 'z';
        else if (position_height == 'S')
            position_height = 'a';
        return position_height;
    }

    bool within_bounds(int i, int j) const {
        return i >= 0 && i < height &&
               j >= 0 && j < width;
    }

    vector<Point2i> get_neighbors(const Point2i& current) const {
        static const Point2i trans[] = {{1,0},{-1,0},{0,1},{0,-1}};
        auto[i,j] = current;
        char height_current = position_height(i,j);
        vector<Point2i> neighbors;
        for (auto[di,dj] : trans) {
            int i_neigh = i + di;
            int j_neigh = j + dj;
            if (within_bounds(i_neigh, j_neigh)) {
                char height_neigh = position_height(i_neigh, j_neigh);
                if (height_neigh - height_current <= 1)
                    neighbors.emplace_back(i_neigh, j_neigh);
            }
        }
        return neighbors;
    }

    Point2i find(char character) const {
        size_t idx = data.find(character);
        if (idx == string::npos)
            return Point2i(-1,-1);
        return Point2i(idx/width,idx%width);
    }
};

ostream& operator<<(ostream& out, const Map& map) {
    for (int i = 0; i < map.height; ++i) {
        for (int j = 0; j < map.width; ++j) {
            out << map(i, j);
        }
        out << '\n';
    }
    return out;
}

ostream& operator<<(ostream& out, const Point2i& point) {
    return out << '(' << get<0>(point) << ", " << get<1>(point) << ')';
}

int bfs(const Map& map) {
    int inf = numeric_limits<int>::max();
    Point2i start = map.find('S');
    Point2i end = map.find('E');
    vector<vector<int>> distance(map.height, vector<int>(map.width, inf));
    queue<Point2i> open;
    open.push(start);
    distance[get<0>(start)][get<1>(start)] = 0;

    while (!open.empty() && distance[get<0>(end)][get<1>(end)] == inf) {
        Point2i current = open.front();
        open.pop();
        for (Point2i neigh : map.get_neighbors(current)) {
            if (distance[get<0>(neigh)][get<1>(neigh)] < inf)
                continue;
            distance[get<0>(neigh)][get<1>(neigh)] =
                distance[get<0>(current)][get<1>(current)] + 1;
            open.emplace(neigh);
        }
    }
    return distance[get<0>(end)][get<1>(end)];
}

int main() {
    Map map;
    map.read(cin);
    cout << bfs(map) << endl;
}
