#include <iostream>
#include <string>
using namespace std;

int main() {
  int x = 0;
  int depth = 0;
  string cmd;
  int arg;
  while (cin >> cmd >> arg) {
    if (cmd == "forward") {
      x += arg;
    }
    else if (cmd == "down") {
      depth += arg;
    }
    else if (cmd == "up") {
      depth -= arg;
    }
  }
  int ans = x*depth;
  cout << ans << endl;
}
