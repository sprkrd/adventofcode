#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

constexpr int A[] = {1, 1, 1, 26, 1, 26, 26, 1, 1, 1, 26, 26, 26, 26};
constexpr int B[] = {10, 14, 14, -13, 10, -13, -7, 11, 10, 13, -4, -9, -13, -9};
constexpr int C[] = {2, 13, 13, 9, 15, 3, 6, 5, 16, 1, 6, 3, 7, 9};

int next_z(int a, int b, int c, int input, int previous_z) {
    int x = (previous_z%26 + b) != input;
    int z = (previous_z/a)*(25*x+1) + (input+c)*x;
    return z;
}

vector<pair<int,int>> trace_back(int a, int b, int c, int z) {
    vector<pair<int,int>> candidates;
    // x = (previous_z%26 + b) != input;
    // z = (previous_z/a)*(25*x+1) + (input+c)*x;
    // z - (input+c)*x = (previous_z/a)*(25*x + 1);
    // x = 1
    //      check (z - (input+c)) % 26 = 0
    //      (z - (input+c))/26 + ?/a = (previous_z/a), ? < a
    //      a*(z - (input+c))/26 + ? = previous_z
    //      max(0, a*(z-(input+c))/26) <= previous_z <= a*(z-(input+c))/26 + (a-1)
    //      check (previous_z%26 + b) != input
    // x = 0
    //      z + ?/a = previous_z/a, ? < a
    //      a*z + ? = previous_z
    //      a*z <= previous_z <= a*z + (a-1)
    //      check (previous_z%26 + b) == input
    for (int input = 1; input <= 9; ++input) {
        {
            // assumption #1, x = 1
            int min_previous_z = max(0, a*(z-(input+c))/26);
            int max_previous_z = a*(z-(input+c))/26 + a-1;
            for (int previous_z = min_previous_z; previous_z <= max_previous_z; ++previous_z) {
                int x = (previous_z%26 + b) != input;
                if ((z - (input+c))%26 != 0) continue;
                if (x != 1) continue;
                candidates.emplace_back(previous_z, input);
                int z_ = next_z(a, b, c, input, previous_z);
                if (z_ != z) {
                    assert(false);
                }
            }
        }
        {
            // assumption #2, x = 0
            int min_previous_z = a*z;
            int max_previous_z = a*z + a-1;
            for (int previous_z = min_previous_z; previous_z <= max_previous_z; ++previous_z) {
                int x = (previous_z%26 + b) != input;
                if (x == 1) continue;
                candidates.emplace_back(previous_z, input);
                int z_ = next_z(a, b, c, input, previous_z);
                if (z_ != z) {
                    assert(false);
                }
            }
        }
    }
    return candidates;
}

uint64_t model() {
    typedef pair<int,int> Point2i;
    map<Point2i,uint64_t> cache;
    function<uint64_t(int,int)> _model = [&](int z, int i) {
        auto [it,inserted] = cache.insert({Point2i(z,i), 0}); 
        if (inserted) {
            uint64_t result = -1;
            if (i > 0) {
                auto candidates = trace_back(A[i-1], B[i-1], C[i-1], z);
                for (auto[prev_z,input] : candidates) {
                    uint64_t m = _model(prev_z, i-1);
                    if (m >= 0) {
                        m = m*10 + input;
                        result = min(result, m);
                    }
                }
            }
            else if (z == 0) {
                result = 0;
            }
            it->second = result;
        }
        return it->second;
    };
    return _model(0, 14);
}

int main(int arg, char* argv[]) {
    int64_t input = model();
    cout << input << endl;

    vector<int> I;
    while (input) {
        I.push_back(input%10);
        input /= 10;
    }
    reverse(I.begin(), I.end());

    int z = 0;
    for (int i = 0; i < 14; ++i) {
        z = next_z(A[i], B[i], C[i], I[i], z);
    }
    cout << z << endl;    
}
