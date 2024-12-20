#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <optional>

typedef std::pair<int, int> Vec2i;

constexpr std::array<Vec2i, 4> kDirections{
    Vec2i(0, -1),
    Vec2i(1, 0),
    Vec2i(0, 1),
    Vec2i(-1, 0)
};

const std::string kDirectionChars = "^>v<";

Vec2i operator+(const Vec2i& lhs, const Vec2i& rhs)
{
    return Vec2i(lhs.first + rhs.first, lhs.second + rhs.second);
}

Vec2i operator-(const Vec2i& lhs, const Vec2i& rhs)
{
    return Vec2i(lhs.first - rhs.first, lhs.second - rhs.second);
}

struct Map
{
    std::string data;
    int height;
    int width;
    
    Map(int width = 0, int height = 0, char defaultChar=' ') :
        data(width*height, defaultChar),
        height(height),
        width(width)
    {
    }

    char& at(int x, int y)
    {
        return data[y*width + x];
    }

    char at(int x, int y) const
    {
        return data[y*width + x];
    }

    char& at(const Vec2i& position)
    {
        return at(position.first, position.second);
    }

    char at(const Vec2i& position) const
    {
        return at(position.first, position.second);
    }

    bool validPosition(const Vec2i& position) const
    {
        return position.first >= 0 && position.first < width &&
               position.second >= 0 && position.second < height;
    }

    std::optional<Vec2i> find(const std::string& chars, const Vec2i& offset = Vec2i(-1, 0)) const
    {
        size_t pos = data.find_first_of(chars, 1+offset.second*width+offset.first);
        if (pos != std::string::npos)
        {
            return Vec2i(pos%width, pos/width);
        }
        return {};
    }

    int count(char c) const
    {
        return std::count(data.begin(), data.end(), c);
    }

    void read(std::istream& in)
    {
        data.clear();
        std::string line;
        height = 0;
        while (getline(in, line) && !line.empty())
        {
            if (data.empty())
            {
                width = line.length();
            }
            data += line;
            ++height;
        }
    }

    void print(std::ostream& out) const
    {
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                out << at(x, y);
            }
            out << '\n';
        }
    }
};

std::istream& operator>>(std::istream& in, Map& map)
{
    map.read(in);
    return in;
}

std::ostream& operator<<(std::ostream& out, const Map& map)
{
    map.print(out);
    return out;
}

Map expand(const Map& map)
{
    Map expanded(map.width*2, map.height);
    for (int y = 0; y < map.height; ++y)
    {
        for (int x = 0; x < map.width; ++x)
        {
            Vec2i position(x,y);
            Vec2i positionNew1(2*x,y);
            Vec2i positionNew2(2*x+1,y);
            if (map.at(position) == '#')
            {
                expanded.at(positionNew1) = expanded.at(positionNew2) = '#';
            }
            else if (map.at(position) == '.')
            {
                expanded.at(positionNew1) = expanded.at(positionNew2) = '.';
            }
            else if (map.at(position) == 'O')
            {
                expanded.at(positionNew1) = '[';
                expanded.at(positionNew2) = ']';
            }
            else
            {
                expanded.at(positionNew1) = '@';
                expanded.at(positionNew2) = '.';
            }
        }
    }
    return expanded;
}

bool stepX(Map& map, Vec2i& robotPosition, char instruction)
{
    Vec2i direction = instruction=='>'? Vec2i(1,0) : Vec2i(-1,0);
    Vec2i targetPosition = robotPosition + direction;
    while (map.at(targetPosition) == '[' || map.at(targetPosition) == ']')
    {
        targetPosition = targetPosition + direction;
    }
    
    bool canMove = map.at(targetPosition) == '.';
    
    if (canMove)
    {
        while (targetPosition != robotPosition)
        {
            Vec2i previousPosition = targetPosition - direction;
            map.at(targetPosition) = map.at(previousPosition);
            targetPosition = previousPosition;
        }
        map.at(robotPosition) = '.';
        robotPosition = robotPosition + direction;
    }
    
    return canMove;
}

bool stepY(Map& map, Vec2i& robotPosition, char instruction)
{
    Vec2i direction = instruction=='v'? Vec2i(0,1) : Vec2i(0,-1);
    
    std::map<Vec2i,char> stagedChanges{{robotPosition,'.'}};
    std::queue<std::pair<Vec2i,char>> queue;
    queue.emplace(robotPosition+direction, '@');
    
    while (!queue.empty())
    {
        auto[position,newValue] = queue.front();
        queue.pop();
        
        if (stagedChanges.count(position))
        {
            continue;
        }
        
        char oldValue = map.at(position);
        
        stagedChanges.emplace(position, newValue);
        
        if (oldValue == '.')
        {
            continue;
        }
        
        if (oldValue == '#')
        {
            return false;
        }
        
        Vec2i adjacentPosition;
        char adjacentValue;
        if (oldValue == '[')
        {
            adjacentPosition = position + Vec2i(1,0);
            adjacentValue = ']';
        }
        else
        {
            adjacentPosition = position + Vec2i(-1,0);
            adjacentValue = '[';
        }
        
        queue.emplace(adjacentPosition, '.');
        queue.emplace(position+direction, oldValue);
        queue.emplace(adjacentPosition+direction, adjacentValue);
    }
    
    robotPosition = robotPosition + direction;
    
    for (const auto&[position,newValue] : stagedChanges)
    {
        map.at(position) = newValue;
    }
    
    return true;
}

bool step(Map& map, Vec2i& robotPosition, char instruction)
{
    if (robotPosition.first == -1)
    {
        robotPosition = *map.find("@");
    }
    if (instruction == '<' || instruction == '>')
    {
        return stepX(map, robotPosition, instruction);
    }
    else
    {
        return stepY(map, robotPosition, instruction);
    }
}

int main()
{
    Map map;
    std::cin >> map;
    
    std::string instructions;
    std::string line;
    while (std::getline(std::cin, line))
    {
        instructions += line;
    }
    
    map = expand(map);
    
    // std::cout << map << std::endl;
    Vec2i robotPosition(-1,-1);
    for (char instruction : instructions)
    {
        step(map, robotPosition, instruction);
        // std::cout << map << std::endl;
    }
    
    int ans = 0;
    auto box = map.find("[");
    while (box)
    {
        ans += box->second*100 + box->first;
        box = map.find("[", *box);
    }
    std::cout << ans << std::endl;
}
