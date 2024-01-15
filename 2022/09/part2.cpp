#include <cassert>
#include <iostream>
#include <set>
#include <utility>
#include <vector>
using namespace std;

typedef pair<int,int> Point2i;

int sign(int x) {
    if (x > 0) return 1;
    else if (x < 0) return -1;
    return 0;
}

struct System {
    vector<Point2i> knots;

    System() : knots(10, Point2i(0,0)) {}

    void step(char dir) {
        switch (dir) {
            case 'R': knots[0].first += 1; break;
            case 'L': knots[0].first -= 1; break;
            case 'U': knots[0].second += 1; break;
            case 'D': knots[0].second -= 1; break;
            default: assert(false);
        }
        // adjust tail
        for (unsigned i = 1; i < knots.size(); ++i) {
            int dx = knots[i-1].first - knots[i].first;
            int dy = knots[i-1].second - knots[i].second;
            bool knot_moves = abs(dx) > 1 || abs(dy) > 1;
            knots[i].first += sign(dx)*knot_moves;
            knots[i].second += sign(dy)*knot_moves;
        }
    }

    const Point2i& tail() const {
        return knots.back();
    }
};

int main() { 
    System system;
    set<Point2i> tail_locations{system.tail()};
    char cmd;
    int qty;
    while (cin >> cmd >> qty) {
        while (qty--) {
            system.step(cmd);
            tail_locations.insert(system.tail());
        }
    }
    cout << tail_locations.size() << endl;
}
