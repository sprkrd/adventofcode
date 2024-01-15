#include <iostream>
#include <stack>
#include <string>
using namespace std;

bool is_closing_bracket(char c) {
    return c == ')' || c == ']' || c == '}' || c == '>';
}

bool is_matching_close_bracket(char c1, char c2) {
    switch (c1) {
        case '(': return c2 == ')';
        case '[': return c2 == ']';
        case '{': return c2 == '}';
        case '<': return c2 == '>';
    }
    return false;
}

int bracket_score(char c) {
    switch (c) {
        case ')': return 3;
        case ']': return 57;
        case '}': return 1197;
        case '>': return 25137;
    }
    return 0;
}

int error_score(const string& line) {
    stack<char> stk;
    for (char c : line) {
        if (is_closing_bracket(c)) {
            if (!stk.empty() && is_matching_close_bracket(stk.top(), c)) {
                stk.pop();
            }
            else {
                return bracket_score(c);
            }
        }
        else {
            stk.push(c);
        }
    }
    return 0;
}

int main() {
    int syntax_error_score = 0;
    string line;
    while (getline(cin, line)) {
        syntax_error_score += error_score(line);
    }
    cout << syntax_error_score << endl;
}
