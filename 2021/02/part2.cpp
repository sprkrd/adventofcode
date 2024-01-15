#include <iostream>
#include <string>
using namespace std;

int main() {
  int x = 0;
  int depth = 0;
  int aim = 0;
  string cmd;
  int arg;
  while (cin >> cmd >> arg) {
    if (cmd == "forward") {
      x += arg;
      depth += aim*arg;
    }
    else if (cmd == "down") {
      aim += arg;
    }
    else if (cmd == "up") {
      aim -= arg;
    }
  }
  int ans = x*depth;
  cout << ans << endl;
}
