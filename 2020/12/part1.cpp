#include <iostream>
#include <string>
using namespace std;

int x = 0, y = 0;
int angle = 0;

istream& read_cmd(istream& in, char& action, int& argument) {
  string cmd_str;
  if (in >> cmd_str) {
    action = cmd_str[0];
    argument = stoi(cmd_str.substr(1));
  }
  return in;
}

void exec(char action, int argument) {
  switch (action) {
    case 'N':
      y += argument;
      break;
    case 'E':
      x += argument;
      break;
    case 'S':
      y -= argument;
      break;
    case 'W':
      x -= argument;
      break;
    case 'L':
      angle = (angle+argument)%360;
      break;
    case 'R':
      angle = (angle-argument+360)%360;
      break;
    default://case 'F':
      switch (angle) {
        case 0:
          x += argument;
          break;
        case 90:
          y += argument;
          break;
        case 180:
          x -= argument;
          break;
        default://case 270:
          y -= argument;
      }
  }
}

int main() {
  char action = 0;
  int argument = 0;
  while (read_cmd(cin,action,argument))
    exec(action,argument);
  cout << x << ' ' << y << ' ' << angle << endl;
  cout << (abs(x)+abs(y)) << endl;
}

