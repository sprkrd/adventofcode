#include <iostream>
#include <string>
#include <fstream>
using namespace std;

constexpr int M = 100;
constexpr int cursor0 = 50;

int main()
{
    int password = 0;
    int cursor = cursor0;
    string line;
    while (getline(cin, line))
    {
        char direction = line[0];
        int amount = stoi(line.substr(1));
        int sign = direction=='R'? 1 : -1;
        int newCursor = cursor + sign*amount;
        if (newCursor >= M) password += newCursor/M;
        else if (newCursor <= 0) password += (cursor>0) - newCursor/M;
        cursor = (newCursor%M+M)%M;
    }
    cout << password << endl;
}
