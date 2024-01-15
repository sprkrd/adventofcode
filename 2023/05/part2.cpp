#include <algorithm>
#include <cassert>
#include <climits>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <tuple>
using namespace std;

typedef uint64_t Uint;

struct Interval {
    Uint lo;
    Uint hi;

    Interval() : lo(1), hi(0) {}

    Interval(Uint lo, Uint hi) : lo(lo), hi(hi) {}

    size_t size() const {
        return hi - lo + 1;
    }

    bool empty() const {
        return lo > hi;
    }
};

void merge_intervals(vector<Interval>& intervals) {
    sort(intervals.begin(), intervals.end(),
            [](const Interval& i1, const Interval& i2) {
                return i1.lo < i2.lo;
            }
    );
    vector<Interval> aux;
    if (!intervals.empty()) {
        aux.push_back(intervals.front());
    }
    for (size_t i = 1; i < intervals.size(); ++i) {
        if (intervals[i].lo > aux.back().hi+1) {
            aux.push_back(intervals[i]);
        }
        else {
            aux.back().hi = max(aux.back().hi, intervals[i].hi);
        }
    }
    intervals = move(aux);
}

ostream& operator<<(ostream& out, const Interval& interval) {
    return out << '[' << interval.lo << ", " << interval.hi << ']';
}

ostream& operator<<(ostream& out, const vector<Interval>& intervals) {
    out << '{';
    for (size_t i = 0; i < intervals.size(); ++i) {
        if (i > 0) {
            out << ", ";
        }
        out << intervals[i];
    }
    return out << '}';
}

struct Range {
    Uint src_begin;
    Uint dst_begin;
    Uint length;

    Interval get_dst_interval(const Interval& src_interval) const {
        Interval src_intersection(max(src_interval.lo, src_begin),
                                  min(src_interval.hi, src_begin+length-1));
        if (!src_intersection.empty()) {
            return Interval(dst_begin+src_intersection.lo-src_begin,
                            dst_begin+src_intersection.hi-src_begin);
        }
        return Interval();
    }
};

struct MapEntry {
    string src;
    string dst;
    vector<Range> ranges;

    void get_dst_intervals(Interval interval, vector<Interval>& out) const {
        // assume ranges are sorted and non-overlapping
        for (const Range& range : ranges) {
            if (interval.empty()) {
                break;
            }
            if (interval.lo > range.src_begin+range.length-1) {
                continue;
            }
            if (interval.lo < range.src_begin) {
                // this is a case in which the intervals aren't explicitly mapped
                out.emplace_back(interval.lo, min(range.src_begin-1, interval.hi));
                interval.lo = range.src_begin;
            }
            auto dst_interval = range.get_dst_interval(interval);
            if (!dst_interval.empty()) {
                out.push_back(dst_interval);
            }
            interval.lo = range.src_begin + range.length;
        }
        // Additional case in which intervals aren't explicitly mapped
        if (!interval.empty()) {
            out.emplace_back(interval.lo, interval.hi);
        }
    }

    vector<Interval> get_dst_intervals(const vector<Interval>& src_intervals) const {
        vector<Interval> out;
        for (const Interval& interval : src_intervals) {
            get_dst_intervals(interval, out);
        }
        merge_intervals(out);
        return out;
    }
};

struct Almanac {
    vector<Uint> seeds;
    vector<MapEntry> map;

    
    pair<string,vector<Interval>> get_dst(const string& src_label,
            const vector<Interval>& src_intervals) const {
        // assume preprocess has been invoked previously
        for (const auto& entry : map) {
            if (entry.src==src_label) {
                return {entry.dst, entry.get_dst_intervals(src_intervals)};
            }
        }
        assert(false);
        return {"", {}};
    }
    
    void preprocess() {
        for (auto& entry : map) {
            sort(entry.ranges.begin(), entry.ranges.end(),
                    [](const Range& r1, const Range& r2) {
                        return r1.src_begin < r2.src_begin;
                    }
            );
        }
    }
};

Uint min_loc(const Almanac& almanac) {
    Uint min_loc = numeric_limits<Uint>::max();
    for (size_t i = 0; i < almanac.seeds.size(); i += 2) {
        string src_label = "seed";
        Interval seed_interval(almanac.seeds[i], almanac.seeds[i] + almanac.seeds[i+1]-1);
        vector<Interval> src_intervals{seed_interval};
        while (src_label != "location") {
            tie(src_label, src_intervals) = almanac.get_dst(src_label, src_intervals);
        }
        min_loc = min(src_intervals.front().lo, min_loc);
    }
    return min_loc;
}

istream& operator>>(istream& in, Almanac& almanac) {
    almanac.seeds.clear();
    almanac.map.clear();

    MapEntry current_entry;

    string line;
    getline(in, line);
    {
        istringstream iss(line);
        string token;
        iss >> token;
        assert(token=="seeds:");
        while (iss >> token) {
            almanac.seeds.push_back(stoull(token));
        }
    }

    while (getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        else if (isdigit(line[0])) {
            istringstream iss(line);
            Range range;
            iss >> range.dst_begin >> range.src_begin >> range.length;
            current_entry.ranges.push_back(range);
        }
        else {
            if (!current_entry.src.empty()) {
                almanac.map.push_back(move(current_entry));
            }
            current_entry.src = line.substr(0, line.find_first_of('-'));
            size_t idx_dst_label = line.find_last_of('-') + 1;
            current_entry.dst = line.substr(idx_dst_label, line.size()-5-idx_dst_label);
            current_entry.ranges.clear();
        }
    }
    if (!current_entry.src.empty()) {
        almanac.map.push_back(move(current_entry));
    }
    return in;
}

ostream& operator<<(ostream& out, const Almanac& almanac) {
    out << "seeds:";
    for (Uint seed : almanac.seeds) {
        out << ' ' << seed;
    }
    out << '\n';
    for (const auto& entry : almanac.map) {
        out << entry.src << '-' << "to" << '-' << entry.dst << " map:\n";
        for (const auto& range : entry.ranges) {
            out << '[' << range.src_begin << ", "
                << (range.src_begin + range.length - 1) << "] -> "
                << '[' << range.dst_begin << ", "
                << (range.dst_begin + range.length - 1) << "]\n";
        }
        out << '\n';
    }
    return out;
}

int main() {
    Almanac almanac;
    cin >> almanac;
    almanac.preprocess();
    cout << min_loc(almanac) << endl;
}
