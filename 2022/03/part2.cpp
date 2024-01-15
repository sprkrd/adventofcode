#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
using namespace std;

typedef set<char> Rucksack;

int main() {
    int result = 0;
    string line;
    while (getline(cin, line)) {
        Rucksack r1(line.begin(), line.end());
        getline(cin, line);
        Rucksack r2(line.begin(), line.end());
        getline(cin, line);
        Rucksack r3(line.begin(), line.end());
        string intersection1, intersection2;
        set_intersection(r1.begin(), r1.end(),
                         r2.begin(), r2.end(),
                         back_inserter(intersection1));
        set_intersection(intersection1.begin(), intersection1.end(),
                         r3.begin(), r3.end(),
                         back_inserter(intersection2));
        assert(intersection2.length() == 1);
        char c = intersection2[0];
        int priority = islower(c)? (c-'a'+1) : (c-'A'+27);
        result += priority;
    }
    cout << result << endl;
}
