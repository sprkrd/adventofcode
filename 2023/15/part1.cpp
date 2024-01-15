#include <iostream>
#include <string>
using namespace std;

size_t HASH(const string& str) {
    size_t seed = 0;
    for (char c : str) {
        seed = (seed + c)*17;
    }
    seed = seed%256;
    return seed;
}

int main() {
    size_t sum = 0;
    string line;
    while (getline(cin,line,',')) {
        if (line.back()=='\n') {
            line.pop_back();
        }
        sum += HASH(line);
    }
    cout << sum << endl;
}
