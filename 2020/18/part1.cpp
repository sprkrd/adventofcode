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
  return tokens;
}

class expression_t {
  public:
    virtual int eval() const = 0;

    virtual ostream& print(ostream& out) const = 0;

    virtual ~expression_t() = default;
};

class atomic_t : public expression_t {
  public:
    atomic_t(int value) : m_value(value) {}

    virtual ostream& print(ostream& out) const override {
      return out << m_value;
    }

    int eval() const override { return m_value; }

  private:
    int m_value;
};

class compound_t : public expression_t {
  public:
    compound_t(unique_ptr<expression_t> left, unique_ptr<expression_t> right,
               char op) : m_left(move(left)), m_right(move(right)), m_op(op) {}

    virtual ostream& print(ostream& out) const override {
      m_left->print(out << '(') << ' ' << m_op;
      m_right->print(out << ' ') << ')';
      return out;
    }

    int eval() const override {
      int l = m_left->eval();
      int r = m_right->eval();
      return m_op == '+'? (l+r) : (l*r);
    }

  private:
    unique_ptr<expression_t> m_left, m_right;
    char m_op;
};

unique_ptr<expression_t> parse(const vector<string>& tokens, unsigned& i) {
  unique_ptr<expression_t> ret;
  while (i < tokens.size()) {
    const string& tok = tokens[i++];
    if (tok == ")")
      return ret;
    else if (tok == "(")
      ret = parse(tokens,i);
    else if (tok[0] == '+' || tok[0] == '*') {
      assert(ret);

      ret = make_unique<compound_t>(move(ret), parse(tokens,i), tok[0]);
    }
    else {
      assert(!ret);
      ret = make_unique<atomic_t>(stoi(tok));
    }
  }
  return ret;
}

ostream& operator<<(ostream& out, const vector<string>& tokens) {
  for (unsigned i = 0; i < tokens.size(); ++i) {
    if (i > 0) out << ',';
    out << tokens[i];
  }
  return out;
}

int eval(const string& expression) {
  auto tokens = tokenize(expression);
  unsigned i = 0;
  auto ast = parse(tokens,i);
  assert(i == tokens.size());
  ast->print(cout) << endl;
  return ast->eval();
}

int main() {
  cout << eval("2 + 2 + 2 * 2") << endl;
  //string line;
  //while (getline(cin,line)) {
    //auto tokens = tokenize(line);
    //cout << tokens << endl;
  //}

}

