#include <algorithm>
#include <cassert>
#include <iostream>
#include <regex>
#include <set>
#include <string>
#include <vector>
using namespace std;

typedef int64_t integer;
typedef pair<integer,integer> Point2i;

integer min_value = 0;
integer max_value = 4000000;

ostream& operator<<(ostream& out, const Point2i& point) {
    auto&&[x,y] = point;
    return out << '(' << x << ',' << y << ')';
}

vector<integer> every_int(const string& str) {
    static const regex rex("-?[0-9]+");
    vector<integer> ans;
    sregex_iterator it(str.begin(), str.end(), rex);
    for (; it != sregex_iterator(); ++it) {
        ans.push_back(stoll(it->str()));
    }
    return ans;
}

struct Interval {
    integer lo;
    integer hi;

    Interval(integer lo = 0, integer hi = 1) : lo(lo), hi(hi) {}

    integer size() const {
        return max(hi - lo + 1, integer(0));
    }

    bool contains(integer x) const {
        return lo <= x && x <= hi;
    }
    
    pair<bool,Interval> merge(const Interval& other) const { 
        Interval result(min(lo,other.lo), max(hi,other.hi));
        bool valid_result = result.size() <= size() + other.size();
        return make_pair(valid_result,result);
    }

    bool operator<(const Interval& other) const {
        return lo<other.lo || (lo==other.lo && hi<other.hi);
    }
};

ostream& operator<<(ostream& out, const Interval& interval) {
    return out << '[' << interval.lo << ',' << interval.hi << ']';
}


struct Sensor {
    Point2i position;
    Point2i closest_beacon;

    integer distance_to_beacon() const {
        auto[x,y] = position;
        auto[x_b,y_b] = closest_beacon;
        return abs(x-x_b) + abs(y-y_b);
    }
};

struct InputData {
    vector<Sensor> sensors;
    vector<Point2i> beacons;
};

InputData read_input(istream& in) {
    InputData input;
    set<Point2i> beacons;
    string line;
    while (getline(in,line)) {
        auto ints = every_int(line);
        Point2i sensor_position(ints[0], ints[1]);
        Point2i beacon_position(ints[2], ints[3]);
        input.sensors.push_back({sensor_position,beacon_position});
        beacons.insert(beacon_position);
    }
    input.beacons.insert(input.beacons.end(), beacons.begin(), beacons.end());
    return input;
}

void merge_overlapping_intervals(vector<Interval>& intervals) {
    sort(intervals.begin(), intervals.end());
    vector<Interval> non_overlapping;
    for (size_t i = 0; i < intervals.size();) {
        Interval current_interval = intervals[i];
        size_t j;
        for (j = i+1; j < intervals.size(); ++j) {
            auto[valid_result,result] = current_interval.merge(intervals[j]);
            if (valid_result) {
                current_interval = result;
            }
            else {
                break;
            }
        }
        non_overlapping.push_back(current_interval);
        i = j;
    }
    intervals = move(non_overlapping);
}

bool find_it(const InputData& input, integer& x, integer y) {
    vector<Interval> intervals;
    for (const Sensor& sensor : input.sensors) {
        auto[x_sensor,y_sensor] = sensor.position;
        integer coverage = sensor.distance_to_beacon();
        integer column_spread = coverage - abs(y-y_sensor);
        Interval interval(max(min_value, x_sensor-column_spread), min(max_value, x_sensor+column_spread));
        if (interval.size() > 0) {
            intervals.emplace_back(x_sensor-column_spread, x_sensor+column_spread);
        }
    }
    merge_overlapping_intervals(intervals);
    if (intervals.size() == 2) {
        assert(intervals[0].hi+2 == intervals[1].lo);
        x = intervals[0].hi + 1;
        return true;
    }
    return false;
}

int main(int argc, char* argv[]) {
    InputData input = read_input(cin);
    for (integer y = min_value; y <= max_value; ++y) {
        integer x;
        if (find_it(input, x, y)) {
            integer ans = x*max_value + y;
            cout << ans << endl;
            return 0;
        }
    }
    assert(false);
}
