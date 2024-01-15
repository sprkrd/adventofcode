#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <numeric>
#include <string>
#include <vector>
using namespace std;

typedef array<int,2> Point2i;
typedef array<int,3> Point3i;

ostream& operator<<(ostream& out, const Point3i& point) {
    return out << point[0] << ',' << point[1] << ',' << point[2];
}

Point3i str_to_point3i(const string& text) {
    size_t fst_comma = text.find_first_of(',');
    size_t snd_comma = text.find_first_of(',', fst_comma+1);
    Point3i p;
    p[0] = stoi(text.substr(0, fst_comma));
    p[1] = stoi(text.substr(fst_comma+1, snd_comma-fst_comma-1));
    p[2] = stoi(text.substr(snd_comma+1));
    return p;
}

Point2i xy_proyection(const Point3i& point3i) {
    return Point2i{point3i[0], point3i[1]};
}

template<class Scalar>
struct Matrix {
    size_t nrows;
    size_t ncols;
    vector<Scalar> data;

    Matrix(size_t nrows, size_t ncols, Scalar default_val = Scalar()) :
        nrows(nrows), ncols(ncols), data(nrows*ncols, default_val) {}

    Scalar at(size_t i, size_t j) const {
        return data[i*ncols + j];
    }

    Scalar& at(size_t i, size_t j) {
        return data[i*ncols + j];
    }
};

template<class Num>
void sort2(Num& a, Num& b) {
    if (b < a) {
        swap(a, b);
    }
}

struct Brick {
    Point3i begin;
    Point3i end;

    void read(const string& text) {
        size_t tilde_idx = text.find_first_of('~');
        string first_part = text.substr(0, tilde_idx);
        string second_part = text.substr(tilde_idx+1);
        begin = str_to_point3i(first_part);
        end = str_to_point3i(second_part);
        sort2(begin[0], end[0]);
        sort2(begin[1], end[1]);
        sort2(begin[2], end[2]);
    }

    vector<Point3i> all_cubes() const {
        vector<Point3i> cubes;
        for (int x = begin[0]; x <= end[0]; ++x) {
            for (int y = begin[1]; y <= end[1]; ++y) {
                for (int z = begin[2]; z <= end[2]; ++z) {
                    cubes.push_back(Point3i{x,y,z});
                }
            }
        }
        return cubes;
    }
};

ostream& operator<<(ostream& out, const Brick& brick) {
    return out << brick.begin << '~' << brick.end;
}


pair<Point3i,Point3i> get_xy_limits(const vector<Brick>& bricks) {
    Point3i min_values;
    Point3i max_values;
    min_values.fill(numeric_limits<int>::max());
    max_values.fill(numeric_limits<int>::min());
    for (const auto& brick : bricks) {
        for (int i = 0; i < 3; ++i) {
            min_values[i] = min(min_values[i], brick.begin[i]);
            max_values[i] = max(max_values[i], brick.end[i]);
        }
    }
    return {min_values, max_values};
}


void settle(vector<Brick>& bricks) {
    // sort by z coordinate
    sort(bricks.begin(), bricks.end(), [](const Brick& a, const Brick& b) {
        return a.begin[2] < b.begin[2];
    });

    auto[min_values,max_values] = get_xy_limits(bricks);
    Matrix<int> height_map(max_values[0]-min_values[0]+1,
                           max_values[1]-min_values[1]+1);
    for (auto& brick : bricks) {
        int max_h = 0;
        auto cubes = brick.all_cubes();
        for (const auto& cube : cubes) {
            int h = height_map.at(cube[0]-min_values[0], cube[1]-min_values[1]);
            max_h = max(h, max_h);
        }
        int distance_to_support = brick.begin[2] - max_h - 1;
        brick.begin[2] -= distance_to_support;
        brick.end[2] -= distance_to_support;
        for (const auto& cube : cubes) {
            height_map.at(cube[0]-min_values[0], cube[1]-min_values[1]) =
                cube[2] - distance_to_support;
        }
    }
}

typedef vector<vector<int>> Graph;

template<class Scalar>
bool contains(const vector<Scalar>& v, Scalar s) {
    return find(v.begin(), v.end(), s) != v.end();
}

Graph support_graph(const vector<Brick>& bricks) {
    Graph graph(bricks.size());
    auto[min_values,max_values] = get_xy_limits(bricks);
    Matrix<pair<int,int>> height_map(max_values[0]-min_values[0]+1,
                                     max_values[1]-min_values[1]+1,
                                     {-1,0});
    for (int i = 0; i < (int)bricks.size(); ++i) {
        for (auto&&[x,y,z] : bricks[i].all_cubes()) {
            auto[h,support] = height_map.at(x,y);
            if (h==z-1 && support!=-1 && support!=i && !contains(graph[i],support)) {
                graph[i].push_back(support);
            }
            height_map.at(x,y) = {z,i};
        }
    }
    return graph;
}

int main() {
    vector<Brick> bricks;
    string line;
    while (getline(cin, line)) {
        bricks.emplace_back();
        bricks.back().read(line);
    }
    settle(bricks);
    auto graph = support_graph(bricks);
    //for (int i = 0; i < (int)graph.size(); ++i) {
        //cout << i << " support(s):";
        //for (int s : graph[i]) {
            //cout << " " << s;
        //}
        //cout << endl;
    //}

    vector<bool> can_be_disintegrated(bricks.size(), true);
    for (int i = 0; i < (int)graph.size(); ++i) {
        if (graph[i].size() == 1) {
            can_be_disintegrated[graph[i].back()] = false;
        }
    }
    int sum = accumulate(can_be_disintegrated.begin(),
                         can_be_disintegrated.end(), 0);
    cout << sum << endl;
}
