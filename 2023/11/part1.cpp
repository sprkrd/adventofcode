#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

typedef pair<int,int> Point2i;

ostream& operator<<(ostream& out, const Point2i& point) {
    return out << '(' << point.first << ", " << point.second << ')';
}

typedef vector<Point2i> Set;

Set read(istream& in) {
    Set set;
    int y = 0;
    string line;
    while (getline(in, line)) {
        for (int x = 0; x < line.size(); ++x) {
            if (line[x] == '#') {
                set.emplace_back(x, y);
            }
        }
        ++y;
    }
    return set;
}

void expand(Set& set) {
    Set out;
    out.reserve(set.size());
    sort(set.begin(), set.end());
    int offset_x = 0;
    out.push_back(set.front());
    for (size_t i = 1; i < set.size(); ++i) {
        offset_x += max(set[i].first - set[i-1].first - 1, 0);
        out.emplace_back(set[i].first + offset_x, set[i].second);
    }
    set = move(out);
    out.clear();
    sort(set.begin(), set.end(),
            [](const Point2i& p1, const Point2i& p2) {
                return p1.second<p2.second;
            }
    );
    int offset_y = 0;
    out.push_back(set.front());
    for (size_t i = 1; i < set.size(); ++i) {
        offset_y += max(set[i].second - set[i-1].second - 1, 0);
        out.emplace_back(set[i].first, set[i].second + offset_y);
    }
    set.swap(out);
}

int sum_sp(const Set& set) {
    int ans = 0;
    for (size_t i = 0; i < set.size(); ++i) {
        for (size_t j = i+1; j < set.size(); ++j) {
            int dx = set[i].first - set[j].first;
            int dy = set[i].second - set[j].second;
            ans += abs(dx) + abs(dy);
        }
    }
    return ans;
}

void print_map(ostream& out, const Set& set) {
    int max_x = 0;
    int max_y = 0;
    for (auto[x,y] : set) {
        max_x = max(x, max_x);
        max_y = max(y, max_y);
    }
    vector<string> map(max_y+1, string(max_x+1, '.'));
    for (auto[x,y] : set) {
        map[y][x] = '#';
    }
    for (const auto& line : map) {
        out << line << '\n'; 
    }
}

int main() {
    auto set = read(cin);
    expand(set);
    cout << sum_sp(set) << endl;
}
