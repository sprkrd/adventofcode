#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Range {
    int lo, hi;

    Range(int lo, int hi) : lo(lo), hi(hi) {}

    bool contains(const Range& other) const {
        return other.lo >= lo && other.hi <= hi;
    }

    bool overlaps(const Range& other) const {
        return max(lo,other.lo) <= min(hi,other.hi);
    }
};

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

int main() {
    int count = 0;
    string line;
    while (getline(cin, line)) {
        auto parts = split(line, ",-");
        Range r1(stoi(parts[0]), stoi(parts[1]));
        Range r2(stoi(parts[2]), stoi(parts[3]));
        count += r1.overlaps(r2);
    }
    cout << count << endl;
}



