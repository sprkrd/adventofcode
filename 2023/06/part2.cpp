#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

typedef uint64_t Uint;


Uint nways(Uint max_time, Uint record_distance) {
    // t * (T-t) > D
    // -t ^2 + T * t - D > 0
    // t^2 - T*t + D < 0
    //        T - sqrt(T^2 - 4*D)  T + sqrt(T^2-4*D)
    // t in ( -------------------, ----------------- )
    //                 2                  2
    //
    
    double delta = max_time*max_time - 4*record_distance;
    assert(delta >= 0);
    double sqrt_delta = sqrt(delta);

    Uint t_low = floor((max_time - sqrt_delta)/2) + 1;
    Uint t_high = ceil((max_time + sqrt_delta)/2) - 1;

    return t_high - t_low + 1;
}

int main() {
    vector<Uint> times;
    vector<Uint> distances;

    string line;
    getline(cin, line);
    line = line.substr(5);
    {
        istringstream iss(line);
        Uint t;
        while (iss >> t) {
            times.push_back(t);
        }
    }

    getline(cin, line);
    line = line.substr(9);
    {
        istringstream iss(line);
        Uint d;
        while (iss >> d) {
            distances.push_back(d);
        }
    }

    assert(times.size() == distances.size());

    Uint ans = nways(times[0], distances[0]);
    for (size_t i = 1; i < times.size(); ++i) {
        ans *= nways(times[i], distances[i]);
    }
    cout << ans << endl;
}
