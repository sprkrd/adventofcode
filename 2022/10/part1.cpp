#include <iostream>
using namespace std;

int main() {
    int ncycle = 1;
    int x = 1;
    int sum_signal_strength = 0;
    int next_interesting_cycle = 20;
    string cmd;
    while (cin >> cmd) {
        int x_prev = x;
        if (cmd == "addx") {
            int arg;
            cin >> arg;
            x += arg;
            ncycle += 2;
        }
        else
            ++ncycle;
        int signal_strength = 0; 
        if (ncycle >= next_interesting_cycle) {
            signal_strength = ncycle > next_interesting_cycle?
                (next_interesting_cycle*x_prev) :
                (next_interesting_cycle*x);
            cout << ncycle << ' ' << x << ' ' << x_prev << ' ' << signal_strength << endl;
            next_interesting_cycle += 40;
        }
        sum_signal_strength += signal_strength;
    }
    cout << sum_signal_strength << endl;
}
