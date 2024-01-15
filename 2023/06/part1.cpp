#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

int nways(int max_time, int record_distance) {
    int ans = 0;
    for (int t = 0; t <= max_time; ++t) {
        int speed = t;
        int remaining_time = max_time - t;
        int distance = speed*remaining_time;
        if (distance > record_distance) {
            ++ans;
        }
    }
    return ans;
}

int main() {
    vector<int> times;
    vector<int> distances;

    string line;
    getline(cin, line);
    line = line.substr(5);
    {
        istringstream iss(line);
        int t;
        while (iss >> t) {
            times.push_back(t);
        }
    }

    getline(cin, line);
    line = line.substr(9);
    {
        istringstream iss(line);
        int d;
        while (iss >> d) {
            distances.push_back(d);
        }
    }

    assert(times.size() == distances.size());

    unsigned long long ans = nways(times[0], distances[0]);
    for (size_t i = 1; i < times.size(); ++i) {
        ans *= nways(times[i], distances[i]);
    }
    cout << ans << endl;
}
