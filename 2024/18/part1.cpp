#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <optional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

constexpr int kMapWidth = 71;
constexpr int kMapHeight = 71;

typedef std::pair<int, int> Vec2i;
typedef std::pair<Vec2i,int> State;

constexpr std::array<Vec2i, 4> kDirections{
    Vec2i(0, -1),
    Vec2i(1, 0),
    Vec2i(0, 1),
    Vec2i(-1, 0)
};

Vec2i operator+(const Vec2i& lhs, const Vec2i& rhs)
{
    return Vec2i(lhs.first + rhs.first, lhs.second + rhs.second);
}

struct Vec2iHasher
{
    size_t operator()(const Vec2i& position) const
    {
        return position.second*kMapWidth + position.first;
    }
};

struct StateHasher
{
    size_t operator()(const State& position) const
    {
        return Vec2iHasher{}(position.first)*(kMapWidth*kMapHeight) + position.second;
    }
};

typedef std::unordered_map<Vec2i,int,Vec2iHasher> Obstacles;
typedef std::unordered_set<State,StateHasher> ClosedSet;
typedef std::queue<State> OpenSet;
typedef std::unordered_map<State,int,StateHasher> DistanceMap;

std::vector<std::string> split(const std::string& str, const std::string& delim=" \n\t") {
    std::vector<std::string> parts;
    size_t pos = 0;
    while (pos < str.length())
    {
        size_t pos_next = str.find_first_of(delim, pos);
        if (pos_next > pos)
        {
            parts.push_back(str.substr(pos, pos_next-pos));
        }
        pos = pos_next == std::string::npos? std::string::npos : pos_next + 1;
    }
    return parts;
}

bool validPosition(const Vec2i& position)
{
    return 0 <= position.first && position.first < kMapWidth &&
           0 <= position.second && position.second < kMapHeight;
}

int bfs(const Obstacles& obstacles, const Vec2i& start, const Vec2i& goal)
{
    State startState = {start, 0};
    OpenSet openSet;
    openSet.push(startState);
    ClosedSet closedSet;
    while (!openSet.empty())
    {
        auto state = openSet.front();
        openSet.pop();
        if (closedSet.count(state))
        {
            continue;
        }
        if (state.first == goal)
        {
            return state.second;
        }
        closedSet.insert(state);
        for (const auto& direction : kDirections)
        {
            State successor = {state.first + direction, state.second + 1};
            auto obstacle = obstacles.find(state.first);
            bool collision = obstacle != obstacles.end() && obstacle->second < 1024;
            if (validPosition(successor.first) && !collision)
            {
                openSet.push(successor);
            }
        }
    }
    return -1;
}

int main()
{
    Obstacles obstacles;
    std::string line;
    while (std::getline(std::cin, line))
    {
        auto parts = split(line, ",");
        obstacles.emplace(Vec2i(stoi(parts[0]), stoi(parts[1])), obstacles.size());
    }
    std::cout << bfs(obstacles, Vec2i(0,0), Vec2i(kMapWidth-1, kMapHeight-1)) << std::endl;
}
