#include <iostream>
using namespace std;

int decode(const string& line) {
    size_t first = line.find_first_of("1234567890");
    size_t last = line.find_last_of("1234567890");
    return (line[first]-'0')*10 + (line[last]-'0');
}

int main() {
    int sum = 0;
    string line;
    while (getline(cin, line)) {
        sum += decode(line);
    }
    cout << sum << endl;
}
