#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

typedef pair<int64_t,int64_t> Point2ll;

ostream& operator<<(ostream& out, const Point2ll& point) {
    return out << '(' << point.first << ", " << point.second << ')';
}

typedef vector<Point2ll> Set;

Set read(istream& in) {
    Set set;
    int64_t y = 0;
    string line;
    while (getline(in, line)) {
        for (int64_t x = 0; x < line.size(); ++x) {
            if (line[x] == '#') {
                set.emplace_back(x, y);
            }
        }
        ++y;
    }
    return set;
}

void expand(Set& set, int64_t factor = 2) {
    Set out;
    out.reserve(set.size());
    sort(set.begin(), set.end());
    int64_t offset_x = 0;
    out.push_back(set.front());
    for (size_t i = 1; i < set.size(); ++i) {
        offset_x += (factor-1)*max(set[i].first - set[i-1].first - 1, 0L);
        out.emplace_back(set[i].first + offset_x, set[i].second);
    }
    set = move(out);
    out.clear();
    sort(set.begin(), set.end(),
            [](const Point2ll& p1, const Point2ll& p2) {
                return p1.second<p2.second;
            }
    );
    int64_t offset_y = 0;
    out.push_back(set.front());
    for (size_t i = 1; i < set.size(); ++i) {
        offset_y += (factor-1)*max(set[i].second - set[i-1].second - 1, 0L);
        out.emplace_back(set[i].first, set[i].second + offset_y);
    }
    set.swap(out);
}

int64_t sum_sp(const Set& set) {
    int64_t ans = 0;
    for (size_t i = 0; i < set.size(); ++i) {
        for (size_t j = i+1; j < set.size(); ++j) {
            int64_t dx = set[i].first - set[j].first;
            int64_t dy = set[i].second - set[j].second;
            ans += abs(dx) + abs(dy);
        }
    }
    return ans;
}

void print_map(ostream& out, const Set& set) {
    int64_t max_x = 0;
    int64_t max_y = 0;
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
    expand(set, 1'000'000);
    cout << sum_sp(set) << endl;
}
