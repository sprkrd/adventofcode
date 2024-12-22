#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <optional>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

constexpr int kMapWidth = 71;
constexpr int kMapHeight = 71;

typedef std::pair<int, int> Vec2i;

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


typedef std::unordered_set<Vec2i,Vec2iHasher> PositionSet;
typedef std::unordered_set<Vec2i,Vec2iHasher> ClosedSet;
typedef std::stack<std::pair<bool,Vec2i>> OpenSet;

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

PositionSet dfs(const PositionSet& obstacles, const Vec2i& start, const Vec2i& goal)
{
    OpenSet openSet;
    openSet.emplace(false, start);
    PositionSet cellsInPath;
    ClosedSet closedSet;
    while (!openSet.empty())
    {
        auto[expanded, position] = openSet.top();
        openSet.pop();
        
        if (expanded)
        {
            cellsInPath.erase(position);
            continue;
        }
        
        if (closedSet.count(position))
        {
            continue;
        }
        
        openSet.emplace(true, position);
        cellsInPath.insert(position);
        closedSet.insert(position);
        
        if (position == goal)
        {
            break;
        }
        
        for (const auto& direction : kDirections)
        {
            Vec2i successor = position + direction;
            if (validPosition(successor) && obstacles.find(successor)==obstacles.end())
            {
                openSet.emplace(false, successor);
            }
        }
    }
    return cellsInPath;
}

int main()
{
    std::vector<Vec2i> obstacles;
    std::string line;
    while (std::getline(std::cin, line))
    {
        auto parts = split(line, ",");
        obstacles.emplace_back(stoi(parts[0]), stoi(parts[1]));
    }
    Vec2i start(0,0);
    Vec2i goal(kMapWidth-1, kMapHeight-1);
    PositionSet obstacleSet;
    auto cellsInPath = dfs(obstacleSet, start, goal);
    int obstacleIndex = -1;
    while (!cellsInPath.empty() && obstacleIndex < (int)obstacles.size())
    {
        ++obstacleIndex;
        obstacleSet.insert(obstacles[obstacleIndex]);
        if (cellsInPath.count(obstacles[obstacleIndex]))
        {
            cellsInPath = dfs(obstacleSet, start, goal);
        }
    }
    auto[obstacleX,obstacleY] = obstacles[obstacleIndex];
    std::cout << obstacleX << ',' << obstacleY << std::endl;
}
