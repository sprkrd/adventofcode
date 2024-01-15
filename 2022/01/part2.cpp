#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

int main() {
    vector<int> calories;
    int running_count = 0;
    string line;
    while (getline(cin,line)) {
        if (line.empty()) {
            calories.push_back(running_count);
            running_count = 0;
        }
        else {
            running_count += stoi(line);
        }
    }
    calories.push_back(running_count);
    sort(calories.begin(), calories.end(), greater<int>());
    int result = calories[0] + calories[1] + calories[2];
    cout << result << endl;
}
