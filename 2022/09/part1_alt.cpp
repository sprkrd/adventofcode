#include <cassert>
#include <iostream>
#include <set>
#include <utility>
#include <vector>
using namespace std;

typedef pair<int,int> Point2i;

struct System {
    Point2i head, tail;

    System() : head(0,0), tail(0,0) {}

    void step(char dir) {
        Point2i prev_head = head;
        switch (dir) {
            case 'R': head.first += 1; break;
            case 'L': head.first -= 1; break;
            case 'U': head.second += 1; break;
            case 'D': head.second -= 1; break;
            default: assert(false);
        }
        // adjust tail
        int dx = head.first - tail.first;
        int dy = head.second - tail.second;
        if (abs(dx) > 1 || abs(dy) > 1)
            tail = prev_head;
    }
};

ostream& operator<<(ostream& out, const System& system) {
    out << "H = (" << system.head.first << ", " << system.head.second << ")\n";
    out << "T = (" << system.tail.first << ", " << system.tail.second << ")\n";
    return out;
}

int main() { 
    System system;
    //cout << system << endl;
    set<Point2i> tail_locations{system.tail};
    char cmd;
    int qty;
    while (cin >> cmd >> qty) {
        //cout << cmd << ' ' << qty << endl;
        while (qty--) {
            system.step(cmd);
            tail_locations.insert(system.tail);
            //cout << system << endl;
        }
    }
    cout << tail_locations.size() << endl;
}
