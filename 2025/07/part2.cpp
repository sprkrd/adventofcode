#include <bits/stdc++.h>
using namespace std;

typedef uint64_t UInt;
typedef pair<UInt,UInt> Point2u;

vector<string> world;
map<Point2u,UInt> beamHeads;
UInt ans = 0;

void ReadWorld()
{
    string line;
    while (getline(cin,line))
    {
        size_t j = line.find('S');
        if (j!=string::npos)
        {
            beamHeads[Point2u(world.size(), j)] = 1;
        }
        world.push_back(move(line));
    }
}

void Step()
{
    decltype(beamHeads) nextBeamHeads;
    for (const auto&[head,nWays] : beamHeads)
    {
        auto[i,j] = head;
        if (i == world.size()-1) ans += nWays;
        else if (world[i+1][j] == '^')
        {
            if (j>0) nextBeamHeads[Point2u(i+1,j-1)] += nWays;
            if (j<world.size()-1) nextBeamHeads[Point2u(i+1,j+1)] += nWays;
        }
        else nextBeamHeads[Point2u(i+1,j)] += nWays;
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
    cout << ans << endl;
}
