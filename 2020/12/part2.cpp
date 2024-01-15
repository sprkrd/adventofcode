#include <iostream>
#include <string>
using namespace std;

int x = 0, y = 0;
int vec_x = 10, vec_y = 1;

int cos(int deg) {
  static const int lu[] = {1,0,-1,0};
  if (deg < 0) deg += 360;
  return lu[deg/90];
}

int sin(int deg) {
  return cos(deg-90);
}

istream& read_cmd(istream& in, char& action, int& argument) {
  string cmd_str;
  if (in >> cmd_str) {
    action = cmd_str[0];
    argument = stoi(cmd_str.substr(1));
  }
  return in;
}

void rot(int& x, int& y, int deg) {
  int x_tmp = x*cos(deg) - y*sin(deg);
  int y_tmp = x*sin(deg) + y*cos(deg);
  x = x_tmp;
  y = y_tmp;
}

void exec(char action, int argument) {
  switch (action) {
    case 'N':
      vec_y += argument;
      break;
    case 'E':
      vec_x += argument;
      break;
    case 'S':
      vec_y -= argument;
      break;
    case 'W':
      vec_x -= argument;
      break;
    case 'L':
      rot(vec_x,vec_y,argument);
      break;
    case 'R':
      rot(vec_x,vec_y,-argument);
      break;
    default://case 'F':
      x += argument*vec_x;
      y += argument*vec_y;
  }
}

int main() {
  char action = 0;
  int argument = 0;
  while (read_cmd(cin,action,argument))
    exec(action,argument);
  cout << x << ' ' << y << endl;
  cout << (abs(x)+abs(y)) << endl;
}

