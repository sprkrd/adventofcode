#include <cassert>
#include <climits>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <tuple>
using namespace std;

typedef uint64_t Uint;

struct Range {
    Uint src_begin;
    Uint dst_begin;
    Uint length;

    Uint get_dst_index(Uint src_index) const {
        if (src_index>=src_begin && src_index<src_begin+length) {
            return dst_begin + src_index - src_begin;
        }
        return Uint(-1);
    }
};

struct MapEntry {
    string src;
    string dst;
    vector<Range> ranges;
};

struct Almanac {
    vector<Uint> seeds;
    vector<MapEntry> map;

    pair<string,Uint> get_dst(const string& src_label, Uint src_index) const {
        for (const auto& entry : map) {
            if (entry.src==src_label) {
                for (const auto& range : entry.ranges) {
                    Uint dst_index = range.get_dst_index(src_index);
                    if (dst_index != Uint(-1)) {
                        return {entry.dst, dst_index};
                    }
                }
                return {entry.dst, src_index};
            }
        }
        return {"", -1};
    }
};

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
            out << range.dst_begin << ' ' << range.src_begin << ' ' << range.length << '\n';
        }
        out << '\n';
    }
    return out;
}

Uint min_loc(const Almanac& almanac) {
    Uint min_loc = numeric_limits<Uint>::max();
    for (Uint seed : almanac.seeds) {
        string src_label = "seed";
        Uint src_idx = seed;
        while (src_label != "location") {
            tie(src_label, src_idx) = almanac.get_dst(src_label, src_idx);
        }
        min_loc = min(src_idx, min_loc);
    }
    return min_loc;
}

int main() {
    Almanac almanac;
    cin >> almanac;
    cout << min_loc(almanac) << endl;
}
