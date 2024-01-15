#include <iostream>
#include <string>
#include <vector>
using namespace std;


struct Range {
    int lo, hi;

    Range(int lo = 0, int hi = -1) : lo(lo), hi(hi) {}

    Range intersect(const Range& other) const {
        return Range(max(lo, other.lo), min(hi, other.hi));
    }

    bool is_empty() const {
        return lo > hi;
    }

    bool operator==(const Range& other) const {
        return lo == other.lo && hi == other.hi;
    }

    unsigned long long size() const {
        return max(0, hi - lo + 1);
    }
};


ostream& operator<<(ostream& out, const Range& range) {
    if (range.is_empty())
        out << "empty";
    else
        out << '[' << range.lo << ',' << range.hi << ']';
    return out;
}


struct Cuboid {

    Range xrange, yrange, zrange;

    Cuboid(Range xrange = Range(), Range yrange = Range(), Range zrange = Range())
        : xrange(xrange), yrange(yrange), zrange(zrange) {}

    bool is_empty() const {
        return xrange.is_empty() || yrange.is_empty() || zrange.is_empty();
    }

    Cuboid intersect(const Cuboid& other) const {
        return Cuboid(xrange.intersect(other.xrange),
                      yrange.intersect(other.yrange),
                      zrange.intersect(other.zrange));
    }

    bool operator==(const Cuboid& other) const {
        return xrange == other.xrange &&
               yrange == other.yrange &&
               zrange == other.zrange;
    }

    vector<Cuboid> subtract(const Cuboid& other) const {
        vector<Cuboid> result;
        Cuboid intersection = intersect(other);
        if (intersection.is_empty())
            result.push_back(*this);
        else {
            Range left_slack(xrange.lo, other.xrange.lo-1);
            if (!left_slack.is_empty())
                result.emplace_back(left_slack, yrange, zrange);
                
            Range right_slack(other.xrange.hi+1, xrange.hi);
            if (!right_slack.is_empty())
                result.emplace_back(right_slack, yrange, zrange);
                
            Range bottom_slack(zrange.lo, other.zrange.lo-1);
            if (!bottom_slack.is_empty())
                result.emplace_back(intersection.xrange, yrange, bottom_slack);
                
            Range top_slack(other.zrange.hi+1, zrange.hi);
            if (!top_slack.is_empty())
                result.emplace_back(intersection.xrange, yrange, top_slack);
                
            Range back_slack(yrange.lo, other.yrange.lo-1);
            if (!back_slack.is_empty())
                result.emplace_back(intersection.xrange, back_slack, intersection.zrange);
                
            Range front_slack(other.yrange.hi+1, yrange.hi);
            if (!front_slack.is_empty())
                result.emplace_back(intersection.xrange, front_slack, intersection.zrange);
        }
        return result;
    }

    unsigned long long volume() const {
        return xrange.size()*yrange.size()*zrange.size();
    }
};

ostream& operator<<(ostream& out, const Cuboid& cuboid) {
    if (cuboid.is_empty())
        out << "Cuboid{empty}";
    else
        out << "Cuboid{x in " << cuboid.xrange
            << ", y in " << cuboid.yrange
            << ", z in " << cuboid.zrange << '}';
    return out;
}

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

pair<bool,Cuboid> parse_line(const string& line) {
    // {on|off, x, [number], [number], y, [number], [number], z, [number], [number]}
    //       0  1         2         3  4         5         6  7         8         9
    auto parts = split(line, " ,.=");
    int xmin = stoi(parts[2]);
    int xmax = stoi(parts[3]);
    int ymin = stoi(parts[5]);
    int ymax = stoi(parts[6]);
    int zmin = stoi(parts[8]);
    int zmax = stoi(parts[9]);
    return make_pair(parts[0]=="on", Cuboid(Range(xmin,xmax), Range(ymin,ymax), Range(zmin,zmax)));
}


int main() {
    vector<Cuboid> disjoint_set;

    string line;
    while(getline(cin, line)) {
        auto[enable,cuboid] = parse_line(line);
        vector<Cuboid> next_disjoint_set;
        for (const auto& disjoint_cuboid : disjoint_set) {
            auto parts = disjoint_cuboid.subtract(cuboid);
            next_disjoint_set.insert(next_disjoint_set.end(), parts.begin(), parts.end());
        }

        if (enable)
            next_disjoint_set.push_back(cuboid);

        disjoint_set.swap(next_disjoint_set);
    }

    unsigned long long volume_on = 0;
    for (const auto& disjoint_cuboid : disjoint_set) {
        volume_on += disjoint_cuboid.volume();
    }
    cout << volume_on << endl;

}
