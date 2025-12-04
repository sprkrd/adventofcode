#include <bits/stdc++.h>
using namespace std;

vector<string> world;
int height;
int width;

void Init()
{
    string line;
    while (getline(cin, line))
    {
        world.push_back(move(line));
    }
    height = world.size();
    width = world[0].size();
}

bool Accessible(int i, int j)
{
    int height = world.size();
    int width = world[0].size();
    int nAdjacent = 0;
    for (int i_ = i-1; i_ <= i+1; ++i_)
    {
        if (i_ < 0 || i_ >= height) continue;
        for (int j_ = j-1; j_ <= j+1; ++j_)
        {
            if (j_ < 0 || j_ >= width) continue;
            if (i==i_ && j==j_) continue;
            nAdjacent += world[i_][j_] == '@';
        }
    }
    return nAdjacent < 4;
}

int Step()
{
    int removed = 0;
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            if (world[i][j] != '@') continue;
            if (Accessible(i,j))
            {
                ++removed;
                world[i][j] = '.';
            }
        }
    }
    return removed;
}

int main()
{
    Init();
    int removedTotal = 0;
    while (int removed = Step())
    {
        removedTotal += removed;
    }
    cout << removedTotal << endl;
}
