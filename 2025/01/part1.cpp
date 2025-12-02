#include <iostream>
#include <string>
#include <fstream>
using namespace std;

int main()
{
    int password = 0;
    int cursor = 50;
    string line;
    while (getline(cin, line))
    {
        char direction = line[0];
        int amount = stoi(line.substr(1));
        int sign = direction=='R'? 1 : -1;
        cursor = (cursor + sign*amount)%100;
        if (cursor < 0) cursor += 100;
        password += cursor==0;
    }
    cout << password << endl;
}
