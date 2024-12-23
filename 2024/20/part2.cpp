#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <optional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

struct Map
{
    std::string data;
    int height;
    int width;

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

std::vector<Vec2i> getPathToEnd(const Map& map)
{
    std::vector<Vec2i> path{*map.find("S")};
    while (map.at(path.back()) != 'E')
    {
        for (const auto& direction : kDirections)
        {
            auto neighbor = path.back() + direction;
            if ((path.size() < 2 || neighbor != path[path.size()-2]) && map.at(neighbor) != '#')
            {
                path.push_back(neighbor);
                break;
            }
        }
    }
    return path;
}

int manhattanDistance(const Vec2i& p1, const Vec2i& p2)
{
    auto[x1,y1] = p1;
    auto[x2,y2] = p2;
    return std::abs(x1-x2) + std::abs(y1-y2);
}

int main()
{
    constexpr int saveAtLeast = 100;
    
    Map map;
    std::cin >> map;
    
    auto pathToEnd = getPathToEnd(map);
    
    // std::vector<int> cheats(pathToEnd.size());
    
    int ans = 0;
    
    for (size_t i = 0; i < pathToEnd.size(); ++i)
    {
        for (size_t j = i+saveAtLeast+2; j < pathToEnd.size(); ++j)
        {
            int mdist = manhattanDistance(pathToEnd[i], pathToEnd[j]);
            int savedTime = (j-i) - mdist;
            if (mdist <= 20 && savedTime >= saveAtLeast)
            {
                ++ans;
            }
        }
    }
    
    std::cout << ans << std::endl;
}
