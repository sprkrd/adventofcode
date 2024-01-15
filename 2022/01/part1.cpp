#include <iostream>
#include <string>
using namespace std;

int main() {
    int max_calories = 0;
    int running_count = 0;
    string line;
    while (getline(cin,line)) {
        if (line.empty()) {
            max_calories = max(max_calories, running_count);
            running_count = 0;
        }
        else {
            running_count += stoi(line);
        }
    }
    max_calories = max(max_calories, running_count);
    cout << max_calories << endl;
}
