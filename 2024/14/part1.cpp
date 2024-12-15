#include <array>
#include <iostream>
#include <vector>

typedef std::pair<int,int> Vec2i;

//constexpr Vec2i kMapDimensions(11, 7);
constexpr Vec2i kMapDimensions(101, 103);

Vec2i operator+(const Vec2i& left, const Vec2i& right)
{
    return Vec2i(left.first + right.first, left.second + right.second);
}

Vec2i operator*(int scalar, const Vec2i& vector)
{
    return Vec2i(scalar*vector.first, scalar*vector.second);
}

int wrapAround(int x, int m)
{
    return (x%m + m)%m;
}

Vec2i wrapAround(const Vec2i& position, const Vec2i& mapDimensions)
{
    return Vec2i(wrapAround(position.first, mapDimensions.first),
                 wrapAround(position.second, mapDimensions.second));
}

struct RobotState
{
    Vec2i position;
    Vec2i velocity;

    void update(int nticks)
    {
        position = wrapAround(position + nticks*velocity, kMapDimensions);
    }

    int getQuadrantIndex() const
    {
        auto[w,h] = kMapDimensions;
        auto[x,y] = position;
        if ((w%2==1 && x==w/2) || (h%2==1 && y==h/2))
        {
            return -1;
        }
        int quadrantIndex = 2*(y>h/2) + (x>w/2);
        return quadrantIndex;
    }
};

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

RobotState parseRobotState(const std::string& line)
{
    auto parts = split(line, " ,=");
    RobotState robotState;
    robotState.position = Vec2i(stoi(parts[1]), stoi(parts[2]));
    robotState.velocity = Vec2i(stoi(parts[4]), stoi(parts[5]));
    return robotState;
}

std::ostream& operator<<(std::ostream& out, const RobotState& robotState)
{
    out << "p=" << robotState.position.first << ',' << robotState.position.second << " "
        << "v=" << robotState.velocity.first << ',' << robotState.velocity.second;
    return out;
}

int main()
{
    std::vector<RobotState> robotStates;
    std::string line;
    while (std::getline(std::cin, line))
    {
        robotStates.push_back(parseRobotState(line));
    }

    std::array<int,4> quadrantCount{0};
    for (auto& robotState : robotStates)
    {
        robotState.update(100);
        int quadrantIndex = robotState.getQuadrantIndex();
        if (quadrantIndex != -1)
        {
            quadrantCount[quadrantIndex]++;
        }
    }

    int64_t ans = 1;
    for (int i = 0; i < 4; ++i)
    {
        ans *= quadrantCount[i];
    }

    std::cout << ans << std::endl;
}
