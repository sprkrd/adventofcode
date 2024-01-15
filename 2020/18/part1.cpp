#include <algorithm>
#include <cassert>
#include <cctype>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

vector<string> tokenize(const string& str) {
  vector<string> tokens;
  size_t pos = 0;
  while (pos < str.length()) {
    size_t pos_next = str.find_first_of("() \t\n", pos);
    if (pos_next > pos)
      tokens.push_back(str.substr(pos, pos_next-pos));
    if (pos_next == string::npos)
      pos = string::npos;
    else {
      if (!isspace(str[pos_next]))
        tokens.emplace_back(1,str[pos_next]);
      pos = pos_next + 1;
    }
  }
  tokens.push_back("");
  return tokens;
}

ostream& operator<<(ostream& out, const vector<string>& tokens) {
  for (unsigned i = 0; i < tokens.size(); ++i) {
    if (i > 0) out << ',';
    out << tokens[i];
  }
  return out;
}

long long eval(const vector<string>& tokens, unsigned& i,
         const string& eof="") {
  long long acc = 0;
  char op = 0;
  while (tokens[i++] != eof) {
    const string& tok = tokens[i-1];
    if (tok == "+" || tok == "*")
      op = tok[0];
    else {
      long long value;
      if (tok == "(")
        value = eval(tokens,i,")");
      else
        value = stoll(tok);
      switch (op) {
        case '+':
          acc += value; break;
        case '*':
          acc *= value; break;
        default:
          acc = value;
      }
      op = 0;
    }
  }
  return acc;
}

long long eval(const string& expression) {
  auto tokens = tokenize(expression);
  unsigned i = 0;
  return eval(tokens,i);
}

int main() {
  long long sum = 0;
  string line;
  while (getline(cin,line)) {
    sum += eval(line);
    cout << sum << endl;
  }
  cout << sum << endl;
}

