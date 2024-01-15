#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <regex>
#include <string>
#include <utility>
#include <vector>
using namespace std;

typedef pair<int,int> Point2i;
typedef pair<int,int> Vel2i;

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

struct Box {
    int xmin, xmax, ymin, ymax;
};

//~ void step(Point2i& position, Vel2i& speed) {
    //~ auto&[x,y] = position;
    //~ auto&[vx,vy] = speed;
    //~ x += vx;
    //~ y += vy;
    //~ if (vx > 0)
        //~ vx -= 1;
    //~ else if (vx < 0)
        //~ vx += 1;
    //~ vy -= 1;
//~ }


int get_highest_height(const Box& target) {
    int ymax_total = 0;
    for (int yspeed = 0; yspeed < -target.ymin; ++yspeed) {
        int ymax = yspeed*(yspeed+1)/2;
        int dist_target = ymax - target.ymax;
        double vy = (-1 + sqrt(1 + 8*dist_target))/2;
        int vy_final = ceil(vy);
        int y_final = ymax - vy_final*(vy_final+1)/2;
        if (y_final >= target.ymin) {
            ymax_total = ymax;
        }
    }
    return ymax_total;
}

int main() {
    string line;
    getline(cin, line);
    
    regex re("x=(\\-?\\d+)\\.\\.(\\-?\\d+), y=(\\-?\\d+)\\.\\.(\\-?\\d+)");
    smatch match;
    assert(regex_search(line, match, re));
    Box target;
    target.xmin = stoi(match[1]);
    target.xmax = stoi(match[2]);
    target.ymin = stoi(match[3]);
    target.ymax = stoi(match[4]);
    //~ target.xmin = 20;
    //~ target.xmax = 30;
    //~ target.ymin = -10;
    //~ target.ymax = -5;
    cout << get_highest_height(target) << endl;
    
}
