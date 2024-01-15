#include <algorithm>
#include <iostream>
#include <stack>
#include <string>
#include <vector>
using namespace std;

bool is_closing_bracket(char c) {
    return c == ')' || c == ']' || c == '}' || c == '>';
}

char closing_bracket(char c) {
    switch (c) {
        case '(': return ')';
        case '[': return ']';
        case '{': return '}';
        case '<': return '>';
    }
    return '\0';
}

unsigned long long bracket_score(char c) {
    switch (c) {
        case ')': return 1;
        case ']': return 2;
        case '}': return 3;
        case '>': return 4;
    }
    return 0;
}

unsigned long long error_score(const string& line) {
    stack<char> stk;
    for (char c : line) {
        if (is_closing_bracket(c)) {
            if (!stk.empty() && closing_bracket(stk.top()) == c) {
                stk.pop();
            }
            else {
                return 0;
            }
        }
        else {
            stk.push(c);
        }
    }
    
    unsigned long long score = 0;
    while (!stk.empty()) {
        score = score*5 + bracket_score(closing_bracket(stk.top()));
        stk.pop();
    }
    return score;
}

int main() {
    vector<unsigned long long> score_v;
    string line;
    while (getline(cin, line)) {
        unsigned long long score = error_score(line);
        if (score != 0) {
            score_v.push_back(score);
        }
    }
    sort(score_v.begin(), score_v.end());
    unsigned long long ans = score_v[score_v.size()/2];
    cout << ans << endl;
}
