#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

typedef int64_t Integer;

bool iszero(const vector<Integer>& seq) {
    return all_of(seq.begin(), seq.end(), [](Integer n) { return n==0; });
}

vector<Integer> diff(const vector<Integer>& seq) {
    vector<Integer> out(seq.size()-1);
    for (size_t i = 0; i < seq.size()-1; ++i) {
        out[i] = seq[i+1] - seq[i];
    }
    return out;
}

Integer predict_next(const vector<Integer>& seq) {
    vector<Integer> levels;
    vector<Integer> seq_diff = seq;
    while (!iszero(seq_diff)) {
        levels.push_back(seq_diff.front());
        seq_diff = diff(seq_diff);
    }
    Integer prediction = 0;
    while (!levels.empty()) {
        prediction = levels.back() - prediction;
        levels.pop_back();
    }
    
    return prediction;
}

int main() {
    Integer sum = 0;
    string line;
    while (getline(cin, line)) {
        istringstream iss(line);
        vector<Integer> seq;
        Integer num;
        while (iss >> num) {
            seq.push_back(num);
        }
        sum += predict_next(seq);
    }
    cout << sum << endl;
}
