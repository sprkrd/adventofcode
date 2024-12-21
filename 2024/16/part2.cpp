#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <string>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <optional>

typedef std::pair<int, int> Vec2i;

constexpr int kInf = std::numeric_limits<int>::max();

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

struct State
{
    Vec2i position;
    Vec2i direction;

    State(const Vec2i& position, const Vec2i& direction) :
        position(position), direction(direction)
    {
    }

    State(const Map& map) : position(*map.find("S")), direction(1,0)
    {
    }

    std::vector<std::pair<int,State>> generateSuccessors(const Map& map) const
    {
        std::vector<std::pair<int,State>> successors;

        auto advancePosition = position + direction;
        if (map.validPosition(advancePosition) && map.at(advancePosition) != '#')
        {
            successors.emplace_back(1, State(advancePosition, direction));
        }

        successors.emplace_back(1'000, State(position, Vec2i(direction.second, -direction.first)));
        successors.emplace_back(1'000, State(position, Vec2i(-direction.second, direction.first)));

        return successors;
    }

    bool isGoal(const Map& map) const
    {
        return map.at(position) == 'E';
    }

    bool isStart(const Map& map) const
    {
        return map.at(position) == 'S' && direction.first==1;
    }

    bool operator==(const State& other) const
    {
        return position == other.position &&
               direction == other.direction;
    }
};

struct Vec2iHasher
{
    size_t operator()(const Vec2i& position) const
    {
        return position.first*1'000'003 + position.second;
    }
};

struct StateHasher
{
    size_t operator()(const State& state) const
    {
        size_t h = Vec2iHasher{}(state.position);
        h = h*4 + 3*(state.direction.second==1) +
                  2*(state.direction.second==-1) +
                    (state.direction.first==-1);
        return h;
    }
};

struct StateSearchInfo
{
    std::vector<State> successors;
    int cost;
    bool closed;

    StateSearchInfo(int cost = kInf) : cost(cost), closed(false)
    {
    }
};

typedef std::unordered_map<State,StateSearchInfo,StateHasher> StateInfoMap;
typedef std::pair<int,State> CostState;

struct CostStateCompare
{
    bool operator()(const CostState& cs1, const CostState& cs2) const
    {
        return cs1.first > cs2.first;
    }
};

StateInfoMap distanceTransform(const Map& map)
{
    std::priority_queue<CostState,std::vector<CostState>,CostStateCompare> open;
    Vec2i finalPosition = *map.find("E");
    StateInfoMap infoMap;
    for (const auto& direction : kDirections)
    {
        State endState(finalPosition, direction);
        open.emplace(0, endState);
        infoMap.emplace(endState, StateSearchInfo(0));
    }
    std::unordered_set<State,StateHasher> closed;

    while (!open.empty())
    {
        auto[cost,state] = open.top();
        open.pop();

        auto& stateInfo = infoMap.find(state)->second;

        if (stateInfo.closed)
        {
            continue;
        }
        if (state.isStart(map))
        {
            break;
        }
        stateInfo.closed = true;
        for (const auto&[actionCost,successor] : state.generateSuccessors(map))
        {
            auto& successorInfo = infoMap[successor];
            int successorCost = cost + actionCost;
            if (successorCost < successorInfo.cost)
            {
                successorInfo.cost = successorCost;
                open.emplace(successorCost, successor);
                successorInfo.successors.clear();
                successorInfo.successors.push_back(state);
            }
            else if (successorCost == successorInfo.cost)
            {
                successorInfo.successors.push_back(state);
            }
        }
    }
    return infoMap;
};

int countCellsInBestPath(const State& initialState, const StateInfoMap& infoMap)
{
    std::unordered_set<Vec2i,Vec2iHasher> cellsInPath;
    std::unordered_set<State,StateHasher> statesInPath;
    std::vector<State> stack{initialState};

    while (!stack.empty())
    {
        auto state = stack.back();
        stack.pop_back();
        if (statesInPath.count(state))
        {
            continue;
        }
        statesInPath.insert(state);
        cellsInPath.insert(state.position);
        const auto& stateInfo = infoMap.find(state)->second;
        for (const auto& successor : stateInfo.successors)
        {
            stack.push_back(successor);
        }
    }
    return cellsInPath.size();
}

int main()
{
    Map map;
    std::cin >> map;
    auto distanceMap = distanceTransform(map);
    std::cout << countCellsInBestPath(State(map), distanceMap) << std::endl;
}
