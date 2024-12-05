#include <array>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

typedef pair<int,int> Vec2i;

bool matches(const vector<string>& matrix, const string& pattern, const Vec2i& dir, const Vec2i& loc)
{
    auto[x,y] = loc;
    auto[dx,dy] = dir;
    int n = pattern.length();
    for (int i = 0; i < n; ++i)
    {
        if (matrix[y+i*dy][x+i*dx] != pattern[i])
        {
            return false;
        }
    }
    return true;
}

int count(const vector<string>& matrix, const string& pattern, const Vec2i& dir)
{
    // x + (n-1)*dir[0] >= 0; x >= max(0, -(n-1)*dir[0])
    // x + (n-1)*dir[0] < width; x < min(width, width - (n-1)*dir[0])
    // y + (n-1)*dir[1] >= 0; y >= max(0, -(n-1)*dir[1])
    // y + (n-1)*dir[1] < height; y < min(height, height - (n-1)*dir[1])
    auto[dx,dy] = dir;
    int width = matrix[0].size();
    int height = matrix.size();
    int n = pattern.length();
    int x_begin = max(0, -(n-1)*dx); 
    int x_end = min(width, width-(n-1)*dx);
    int y_begin = max(0, -(n-1)*dy);
    int y_end = min(height, height-(n-1)*dy);
    int ans = 0;
    for (int y = y_begin; y < y_end; ++y)
    {
        for (int x = x_begin; x < x_end; ++x)
        {
            ans += matches(matrix, pattern, dir, Vec2i(x, y));
        }
    }
    return ans;
}

int count(const vector<string>& matrix, const string& pattern)
{
    constexpr array<Vec2i,8> directions = {
        Vec2i{ 1, 0},
        Vec2i{-1, 0},
        Vec2i{ 0, 1},
        Vec2i{ 0,-1},
        Vec2i{ 1, 1},
        Vec2i{-1,-1},
        Vec2i{ 1,-1},
        Vec2i{-1, 1}
    };
    int ans = 0;
    for (const auto& dir : directions)
    {
        ans += count(matrix, pattern, dir);
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
    cout << count(matrix, "XMAS") << endl;
}
