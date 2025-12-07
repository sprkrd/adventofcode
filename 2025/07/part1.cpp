#include <bits/stdc++.h>
using namespace std;

typedef pair<unsigned,unsigned> Point2u;

vector<string> world;
set<Point2u> beamHeads;
int nSplits = 0;

void ReadWorld()
{
    string line;
    while (getline(cin,line))
    {
        size_t j = line.find('S');
        if (j!=string::npos)
        {
            beamHeads.emplace(world.size(), j);
        }
        world.push_back(move(line));
    }
}

void Step()
{
    set<Point2u> nextBeamHeads;
    for (auto[i,j] : beamHeads)
    {
        if (i == world.size()-1) continue;
        if (world[i+1][j] == '^')
        {
            if (j>0) nextBeamHeads.emplace(i+1,j-1);
            if (j<world.size()-1) nextBeamHeads.emplace(i+1,j+1);
            ++nSplits;
        }
        else nextBeamHeads.emplace(i+1,j);
    }
    beamHeads = move(nextBeamHeads);
}

void Simulate()
{
    while (!beamHeads.empty())
    {
        Step();
    }
}

int main()
{
    ReadWorld();
    Simulate();
    cout << nSplits << endl;
}
