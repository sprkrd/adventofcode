#include <cassert>
#include <iostream>
#include <stack>
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
  return tokens;
}

ostream& operator<<(ostream& out, const vector<string>& tokens) {
  for (unsigned i = 0; i < tokens.size(); ++i) {
    if (i > 0) out << ' ';
    out << tokens[i];
  }
  return out;
}

bool is_operator(const string& token) {
  return token == "+" || token == "*";
}

int precedence(const string& token) {
  return 2*(token=="+") + (token=="*");
}

// performs Dijkstra's shunting-yard algorithm to compute reverse
// polish notation of an expression.
vector<string> rpn(const string& expression) {
  auto tokens = tokenize(expression);
  vector<string> output;
  stack<string> operator_stack;
  for (unsigned i = 0; i < tokens.size(); ++i) {
    if (is_operator(tokens[i])) {
      while (!operator_stack.empty() &&
             precedence(tokens[i]) <=
             precedence(operator_stack.top())) {
        output.push_back(operator_stack.top());
        operator_stack.pop();
      }
      operator_stack.push(tokens[i]);
    }
    else if (tokens[i] == "(")
      operator_stack.push(tokens[i]);
    else if (tokens[i] == ")") {
      while (operator_stack.top() != "(") {
        output.push_back(operator_stack.top());
        operator_stack.pop();
      }
      operator_stack.pop();
    }
    else
      output.push_back(tokens[i]);
  }
  while (!operator_stack.empty()) {
    output.push_back(operator_stack.top());
    operator_stack.pop();
  }
  return output;
}

long long eval(const string& expression) {
  auto rpnexp = rpn(expression);
  stack<long long> s;
  for (const auto& tok : rpnexp) {
    if (is_operator(tok)) {
      long long op2 = s.top(); s.pop();
      long long op1 = s.top(); s.pop();
      if (tok == "+")
        s.push(op1+op2);
      else
        s.push(op1*op2);
    }
    else
      s.push(stoll(tok));
  }
  assert(s.size() == 1);
  return s.top();
}


int main() {
  long long sum = 0;
  string line;
  while (getline(cin,line))
    sum += eval(line);
  cout << sum << endl;
}

