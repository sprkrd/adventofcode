#include <cassert>
#include <iostream>
#include <unordered_set>
#include <utility>
#include <string>
using namespace std;

typedef pair<int,int> Point2i;

ostream& operator<<(ostream& out, const Point2i& point) {
    return out << '(' << get<0>(point) << ',' << get<1>(point) << ')';
}

namespace std {
    
    template<> struct hash<Point2i> {
        size_t operator()(const Point2i& point) const {
            hash<int> h;
            return h(get<0>(point))*16383 + h(get<1>(point));
        }
    };
}

struct SparseImage {
    unordered_set<Point2i> data;
    char default_value;
    
    typedef unordered_set<Point2i>::const_iterator const_iterator;
    
    const_iterator begin() const {
        return data.begin();
    }
    
    const_iterator end() const {
        return data.end();
    }
    
    void set(const Point2i& point, char value) {
        if (value == default_value) {
            data.erase(point);
        }
        else {
            data.insert(point);
        }
    }
    
    char operator()(const Point2i& point) const {
        auto it = data.find(point);
        if (it != data.end()) {
            return default_value == '.'? '#' : '.';
        }
        return default_value;
    }
    
    void draw(ostream& out) const {
        int min_x, max_x, min_y, max_y;
        min_x = max_x = min_y = max_y = 0;
        for (auto[x,y] : data) {
            min_x = min(x,min_x);
            max_x = max(x,max_x);
            min_y = min(y,min_y);
            max_y = max(y,max_y);
        }
        for (int y = min_y; y <= max_y; ++y) {
            for (int x = min_x; x <= max_x; ++x) {
                out << (*this)(Point2i(x,y));
            }
            out << '\n';
        }
    }
};

ostream& operator<<(ostream& out, const SparseImage& img) {
    img.draw(out);
    return out;
}

array<Point2i,9> get_neighborhood(const Point2i& point) {
    array<Point2i,9> neighborhood;
    auto[x,y] = point;
    int index = 0;
    for (int y_neigh = y-1; y_neigh <= y+1; ++y_neigh) {
        for (int x_neigh = x-1; x_neigh <= x+1; ++x_neigh) {
            neighborhood[index++] = Point2i{x_neigh, y_neigh};
        }
    }
    return neighborhood;
}

int neighborhood_to_index(const SparseImage& img, const array<Point2i,9>& neighborhood) {
    int index = 0;
    for (const auto& point : neighborhood) {
        index = index*2 + (img(point)=='#');
    }
    return index;
}

SparseImage enhance(const SparseImage& img, const string& algorithm) {
    SparseImage ans;
    if (img.default_value == '#' && algorithm.back() == '.') {
        ans.default_value = '.';
    }
    else if (img.default_value == '.' && algorithm[0] == '#') {
        ans.default_value = '#';
    }
    else {
        ans.default_value = img.default_value;
    }
    unordered_set<Point2i> points_of_interest;
    for (const auto& point : img) {
        for (const auto& point_neigh : get_neighborhood(point)) {
            points_of_interest.insert(point_neigh);
        }
    }
    
    for (const auto& point : points_of_interest) {
        auto neighborhood = get_neighborhood(point);
        int index = neighborhood_to_index(img, neighborhood);
        ans.set(point, algorithm[index]);
    }
    
    return ans;
}

int main() {
    SparseImage img;
    img.default_value = '.';
    string line, algorithm;
    getline(cin, algorithm);
    getline(cin, line);
    int y = 0;
    while (getline(cin, line)) {
        for (int x = 0; x < line.length(); ++x) {
            img.set(Point2i(x,y), line[x]);
        }
        ++y;
    }
    //~ cout << img << endl;
    for (int i = 0; i < 50; ++i) {
        img = enhance(img, algorithm);
    }
    assert(img.default_value == '.');
    cout << img.data.size() << endl;
}
