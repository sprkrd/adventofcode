#include <algorithm>
#include <array>
#include <limits>
#include <iostream>
#include <optional>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

typedef std::pair<int,int> Vec2i;

constexpr int kNumberOfDirectionalKeyboards = 3;
constexpr int kInf = std::numeric_limits<int>::max();

typedef std::array<Vec2i,kNumberOfDirectionalKeyboards-1> DirectionalKeyboardPositions;

const std::array<std::string, 4> kNumericKeyboard{
    "789",
    "456",
    "123",
    " 0A"
};

const std::array<std::string, 2> kDirectionalKeyboard{
    " ^A",
    "<v>"
};

const std::string kActions = "<>^vA";

Vec2i& operator+=(Vec2i& l, const Vec2i& r)
{
    l.first += r.first;
    l.second += r.second;
    return l;
}

Vec2i operator+(const Vec2i& v1, const Vec2i& v2)
{
    return Vec2i(v1.first+v2.first, v1.second+v2.second);
}

Vec2i getDirection(char directionChar)
{
    switch (directionChar)
    {
        case '>':
            return Vec2i(1,0);
        case '<':
            return Vec2i(-1,0);
        case '^':
            return Vec2i(0,-1);
        default:
            return Vec2i(0,1);
    }
}

template<size_t N>
Vec2i findChar(const std::array<std::string,N>& keyboard, char character)
{
    for (size_t y = 0; y < keyboard.size(); ++y)
    {
        for (size_t x = 0; x < keyboard[y].length(); ++x)
        {
            if (keyboard[y][x] == character)
            {
                return Vec2i(x, y);
            }
        }
    }
    return Vec2i(-1,-1);
}

bool isValidPosition(const Vec2i& position, int w, int h, int forbiddenX, int forbiddenY)
{
    auto[x,y] = position;
    if (x == forbiddenX && y == forbiddenY)
    {
        return false;
    }
    if (x < 0 || x >= w || y < 0 || y >= h)
    {
        return false;
    }
    return true;
}

struct State
{
    Vec2i numericKeyboardPosition;
    DirectionalKeyboardPositions directionalKeyboardPositions;
    
    State(char numericKeyboardChar = 'A') :
        numericKeyboardPosition(findChar(kNumericKeyboard, numericKeyboardChar))
    {
        directionalKeyboardPositions.fill(findChar(kDirectionalKeyboard, 'A'));
    }
    
    bool operator==(const State& other) const
    {
        return numericKeyboardPosition == other.numericKeyboardPosition &&
               directionalKeyboardPositions == other.directionalKeyboardPositions;
    }
    
    bool operator!=(const State& other) const
    {
        return !(*this == other);
    }
    
    bool isValidState() const
    {
        if (!isValidPosition(numericKeyboardPosition, 3, 4, 0, 3))
        {
            return false;
        }
        for (const auto& directionalPosition : directionalKeyboardPositions)
        {
            if (!isValidPosition(directionalPosition, 3, 2, 0, 0))
            {
                return false;
            }
        }
        return true;
    }
    
    std::optional<State> step(char action)
    {
        State successor = *this;
        int keyboard = directionalKeyboardPositions.size()-1;
        while (action == 'A' && keyboard >= 0)
        {
            auto[x,y] = successor.directionalKeyboardPositions[keyboard];
            action = kDirectionalKeyboard[y][x];
            --keyboard;
        }
        if (action != 'A')
        {
            if (keyboard == -1)
            {
                successor.numericKeyboardPosition += getDirection(action);
            }
            else
            {
                successor.directionalKeyboardPositions[keyboard] += getDirection(action);
            }
        }
        if (successor.isValidState())
        {
            return successor;
        }
        return {};
    }
    
    void display(std::ostream& out) const
    {
        out << "[" << kNumericKeyboard[numericKeyboardPosition.second][numericKeyboardPosition.first] << "] ";
        out << "(" << numericKeyboardPosition.first << ", " << numericKeyboardPosition.second << ")";
        
        for (auto[x,y] : directionalKeyboardPositions)
        {
            out << " [" << kDirectionalKeyboard[y][x] << "]";
            out << " (" << x << ", " << y << ")";
        }
        out << '\n';
    }
};

namespace std
{

template<>
struct hash<State>
{
    size_t operator()(const State& state) const
    {
        size_t h = state.numericKeyboardPosition.second*3 + state.numericKeyboardPosition.first;
        size_t w = 12;
        for (const auto&[x,y] : state.directionalKeyboardPositions)
        {
            h = w*12 + y*3 + x;
            w = 6;
        }
        return h;
    }
};

}

struct StateInfo
{
    int cost;
    char leadingAction;
    State parent;
    
    StateInfo(const State& parent, char leadingAction, int cost = kInf) :
        cost(cost),
        leadingAction(leadingAction),
        parent(parent)
    {
    }
};

typedef std::unordered_map<State,StateInfo> DistanceMap;

DistanceMap distanceTransform(const State& state)
{
    std::queue<std::pair<State,int>> openSet;
    openSet.emplace(state, 0);
    std::unordered_set<State> closedSet;
    DistanceMap distanceMap;
    distanceMap.emplace(state, StateInfo(state, ' ', 0));
    while (!openSet.empty())
    {
        auto[state,distance] = openSet.front();
        openSet.pop();
        if (closedSet.count(state))
        {
            continue;
        }
        closedSet.insert(state);
        for (char action : kActions)
        {
            int successorCost = distance + 1;
            auto successor = state.step(action);
            if (!successor)
            {
                continue;
            }
            bool inserted = distanceMap.emplace(*successor, StateInfo(state, action, successorCost)).second;
            if (inserted)
            {
                openSet.emplace(*successor, successorCost);
            }
        }
    }
    return distanceMap;
}

std::string rebuildSequence(const DistanceMap& distanceMap, const State& state, char nextKey)
{
    State currentState(nextKey);
    std::string sequence = "A";
    while (currentState != state)
    {
        const auto& stateInfo = distanceMap.find(currentState)->second;
        sequence.push_back(stateInfo.leadingAction);
        currentState = stateInfo.parent;
    }
    std::reverse(sequence.begin(), sequence.end());
    return sequence;
}

struct Optimizer
{
    
    std::unordered_map<State,DistanceMap> cache;
    
    const DistanceMap& retrieveDistanceMap(const State& state)
    {
        auto it = cache.find(state);
        if (it == cache.end())
        {
            it = cache.emplace(state, distanceTransform(state)).first;
        }
        return it->second;
    }
    
    std::string getSequence(const State& state, char nextKey)
    {
        
        const auto& distanceMap = retrieveDistanceMap(state);
        return rebuildSequence(distanceMap, state, nextKey);
    }
    
    int getSequenceLength(const State& state, char nextKey)
    {
        const auto& distanceMap = retrieveDistanceMap(state);
        return distanceMap.find(State(nextKey))->second.cost + 1;
    }
    
};



// Vec2i operator-(const Vec2i& l, const Vec2i& r)
// {
    // return Vec2i(l.first - r.first, l.second - r.second);
// }

// std::string findSequenceFirstXThenY(int dx, int dy)
// {
    // std::string sequence;
    // for (int i = 0; i < std::abs(dx); ++i)
    // {
        // sequence.push_back(dx>0? '>' : '<');
    // }
    // for (int i = 0; i < std::abs(dy); ++i)
    // {
        // sequence.push_back(dy>0? 'v' : '^');
    // }
    // sequence.push_back('A');
    // return sequence;
// }

// std::string findSequenceFirstYThenX(int dx, int dy)
// {
    // std::string sequence;
    // for (int i = 0; i < std::abs(dy); ++i)
    // {
        // sequence.push_back(dy>0? 'v' : '^');
    // }
    // for (int i = 0; i < std::abs(dx); ++i)
    // {
        // sequence.push_back(dx>0? '>' : '<');
    // }
    // sequence.push_back('A');
    // return sequence;
// }

// enum class Order { arbitrary, firstX, firstY };

// // Numeric keypad
// // +---+---+---+
// // | 7 | 8 | 9 |
// // +---+---+---+
// // | 4 | 5 | 6 |
// // +---+---+---+
// // | 1 | 2 | 3 |
// // +---+---+---+
// //     | 0 | A |
// //     +---+---+



// struct Keyboard
// {   
    // virtual Vec2i findCoordinates(char character) const = 0;
    
    // virtual std::string findSequence(char currentChar, char targetChar, Order order = Order::arbitrary) const = 0;
    
    // virtual ~Keyboard() = default;
// };

// struct NumericKeyboard : public Keyboard
// {
    // Vec2i findCoordinates(char character) const override
    // {
        // if (character == '0')
        // {
            // return Vec2i(1, 3);
        // }
        // if (character == 'A')
        // {
            // return Vec2i(2, 3);
        // }
        // int number = character - '1';
        // return Vec2i(number%3, 2-number/3);
    // }
    
    // std::string findSequence(char currentChar, char targetChar, Order order) const override
    // {
        // Vec2i targetPos = findCoordinates(targetChar);
        // Vec2i currentPos = findCoordinates(currentChar);
        // auto[dx,dy] = targetPos - currentPos;
        
        // std::string sequence;
        // if (targetPos.first == 0 && currentPos.second == 3)
        // {
            // if (order != Order::firstX)
            // {
                // sequence = findSequenceFirstYThenX(dx, dy);
            // }
        // }
        // else if (targetPos.second == 3 && currentPos.first == 0)
        // {
            // if (order != Order::firstY)
            // {
                // sequence = findSequenceFirstXThenY(dx, dy);
            // }
        // }
        // else
        // {   
            // sequence = order==Order::firstX? findSequenceFirstXThenY(dx, dy) :
                                             // findSequenceFirstYThenX(dx, dy);
        // }
        // return sequence;
    // }
// };

// // Directional keypad
// //     +---+---+
// //     | ^ | A |
// // +---+---+---+
// // | < | v | > |
// // +---+---+---+

// struct DirectionalKeyboard : public Keyboard
// {
    // Vec2i findCoordinates(char character) const override
    // {
        // switch (character)
        // {
            // case 'A':
                // return Vec2i(2, 0);
            // case '^':
                // return Vec2i(1, 0);
            // case '>':
                // return Vec2i(2, 1);
            // case 'v':
                // return Vec2i(1, 1);
            // default: // case '<'
                // return Vec2i(0, 1);
        // }
    // }
    
    // std::string findSequence(char currentChar, char targetChar, Order order = Order::arbitrary) const override
    // {
        // Vec2i targetPos = findCoordinates(targetChar);
        // Vec2i currentPos = findCoordinates(currentChar);
        // auto[dx,dy] = targetPos - currentPos;
        
        // std::string sequence;
        // if (targetPos.first == 0 && currentPos.second == 0)
        // {
            // if (order != Order::firstX)
            // {
                // sequence = findSequenceFirstYThenX(dx, dy);
            // }
        // }
        // else if (targetPos.second == 0 && currentPos.first == 0)
        // {
            // if (order != Order::firstY)
            // {
                // sequence = findSequenceFirstXThenY(dx, dy);
            // }
        // }
        // else
        // {
            // sequence = order==Order::firstX? findSequenceFirstXThenY(dx, dy) :
                                             // findSequenceFirstYThenX(dx, dy);
        // }
        // return sequence;
    // }
// };


// std::string getFinalSequence(const std::string& key)
// {
    // // NumericKeyboard numericKeyboard;
    // // DirectionalKeyboard directionalKeyboard1;
    // // DirectionalKeyboard directionalKeyboard2;
    
    // // std::string sequence;
    // // for (char c : key)
    // // {
        // // std::string lastSegment1 = numericKeyboard.findShortestSequence(c, directionalKeyboard1.currentChar);
        // // numericKeyboard.currentChar = c;
        // // for (char d1 : lastSegment1)
        // // {
            // // std::string lastSegment2 = directionalKeyboard1.findShortestSequence(d1, directionalKeyboard2.currentChar);
            // // directionalKeyboard1.currentChar = d1;
            // // for (char d2 : lastSegment2)
            // // {
                // // sequence += directionalKeyboard2.findShortestSequence(d2);
                // // directionalKeyboard2.currentChar = d2;
            // // }
        // // }
    // // }
    // std::string sequence;
    // return sequence;
// }

// int complexity(const std::string& key)
// {
    // auto sequence = getFinalSequence(key);
    // int numericCode = std::stoi(key.substr(0, key.size()-1));
    // return numericCode*sequence.size();
// }

int main()
{
    
    int ans = 0;
    
    Optimizer optimizer;
    
    std::string key;
    while (std::cin >> key)
    {
        std::string sequence;
        State state;
        for (char c : key)
        {
            auto subSequence = optimizer.getSequence(state, c);
            state = State(c);
            sequence += subSequence;
        }
        std::cout << key << ": " << sequence << std::endl;
        
        ans += std::stoi(key.substr(0, key.length()-1))*sequence.length();
    }
    std::cout << ans << std::endl;
}
