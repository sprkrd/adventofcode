#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
using namespace std;

typedef set<char> Compartment;

int main() {
    int result = 0;
    string line;
    while (getline(cin, line)) {
        Compartment c1(line.begin(), line.begin()+line.length()/2);
        Compartment c2(line.begin()+line.length()/2, line.end());
        string intersection;
        set_intersection(c1.begin(), c1.end(),
                         c2.begin(), c2.end(),
                         back_inserter(intersection));
        for (char c : intersection) {
            int priority = islower(c)? (c-'a'+1) : (c-'A'+27);
            result += priority;
        }
    }
    cout << result << endl;
}
