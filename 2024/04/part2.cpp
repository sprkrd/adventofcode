#include <array>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

typedef pair<int,int> Vec2i;

bool matches(const vector<string>& matrix, const Vec2i& loc)
{
    auto[x,y] = loc;
    if (matrix[y][x] != 'A')
    {
        return false;
    }
    if (matrix[y-1][x-1] == 'M' && matrix[y-1][x+1] == 'M' &&
        matrix[y+1][x-1] == 'S' && matrix[y+1][x+1] == 'S')
    {
        return true;
    }
    if (matrix[y-1][x-1] == 'M' && matrix[y-1][x+1] == 'S' &&
        matrix[y+1][x-1] == 'M' && matrix[y+1][x+1] == 'S')
    {
        return true;
    }
    if (matrix[y-1][x-1] == 'S' && matrix[y-1][x+1] == 'S' &&
        matrix[y+1][x-1] == 'M' && matrix[y+1][x+1] == 'M')
    {
        return true;
    }
    if (matrix[y-1][x-1] == 'S' && matrix[y-1][x+1] == 'M' &&
        matrix[y+1][x-1] == 'S' && matrix[y+1][x+1] == 'M')
    {
        return true;
    }
    return false;
}

int count(const vector<string>& matrix)
{
    int width = matrix[0].length();
    int height = matrix.size();
    int ans = 0;
    for (int y = 1; y < height-1; ++y)
    {
        for (int x = 1; x < width-1; ++x)
        {
            ans += matches(matrix, Vec2i(x, y));
        }
    }
    return ans;
}

int main() {
    vector<string> matrix;
    string line;
    while (getline(cin, line))
    {
        matrix.push_back(move(line));
    }
    cout << count(matrix) << endl;
}
