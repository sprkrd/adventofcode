#include <iostream>
#include <string>
using namespace std;

constexpr int k_marker_size = 14;

int main() {
    string data;
    cin >> data;
    int char_count[127] = {0};
    int num_repeated_chars = 0;
    unsigned i;
    for (i = 0; i < data.length() && (i<k_marker_size||num_repeated_chars); ++i) {
        num_repeated_chars -= i>=k_marker_size && (--char_count[(int)data[i-k_marker_size]])==1;
        num_repeated_chars += (++char_count[(int)data[i]]) == 2;

    }
    cout << i << endl;
}
