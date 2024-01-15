#include <algorithm>
#include <iostream>
#include <regex>
#include <unordered_map>
using namespace std;


int decode(string line) {
    const static regex re1("one|two|three|four|five|six|seven|eight|nine|zero|[0-9]");
    const static regex re2("eno|owt|eerht|ruof|evif|xis|neves|thgie|enin|orez|[0-9]");
    const static unordered_map<string,int> word2int = {
        {"one", 1},
        {"two", 2},
        {"three", 3},
        {"four", 4},
        {"five", 5},
        {"six", 6},
        {"seven", 7},
        {"eight", 8},
        {"nine", 9},
        {"zero", 0},
        {"0", 0},
        {"1", 1},
        {"2", 2},
        {"3", 3},
        {"4", 4},
        {"5", 5},
        {"6", 6},
        {"7", 7},
        {"8", 8},
        {"9", 9}
    };
    smatch m;
    regex_search(line, m, re1);
    int first = word2int.find(m.str())->second;
    reverse(line.begin(), line.end());
    regex_search(line, m, re2);
    auto str2 = m.str();
    reverse(str2.begin(), str2.end());
    int last = word2int.find(str2)->second;
    return first*10 + last;
}

int main() {
    int sum = 0;
    string line;
    while (getline(cin, line)) {
        sum += decode(line);
    }
    cout << sum << endl;
}
