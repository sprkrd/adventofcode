#include <array>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <utility>

constexpr int kNumberOfRobots = 25;

typedef std::pair<int,int> Vec2i;

struct Keyboard
{
    int width;
    int height;
    std::string keys;

    Keyboard(int width, int height, const std::string& keys) :
        width(width), height(height), keys(keys)
    {
    }

    Vec2i find(char c) const
    {
        size_t pos = keys.find_first_of(c);
        if (pos == std::string::npos)
        {
            return Vec2i(-1,-1);
        }
        return Vec2i(pos%width, pos/width);
    }

    char at(const Vec2i& position) const
    {
        return keys[position.first + position.second*width];
    }

    virtual bool isXYRoutePossible(const Vec2i& start, const Vec2i& end) const = 0;

    virtual bool isYXRoutePossible(const Vec2i& start, const Vec2i& end) const = 0;

    virtual ~Keyboard() = default;
};

struct NumericKeyboard : public Keyboard
{
    NumericKeyboard() : Keyboard(3, 4, "789456123 0A")
    {
    }

    bool isXYRoutePossible(const Vec2i& start, const Vec2i& end) const override
    {
        auto[xStart,yStart] = start;
        auto[xEnd,yEnd] = end;
        return yStart!=3 || xEnd!=0;
    }

    bool isYXRoutePossible(const Vec2i& start, const Vec2i& end) const override
    {
        auto[xStart,yStart] = start;
        auto[xEnd,yEnd] = end;
        return xStart!=0 || yEnd!=3;
    }
};

struct DirectionalKeyboard : public Keyboard
{
    DirectionalKeyboard() : Keyboard(3, 2, " ^A<v>")
    {
    }

    bool isXYRoutePossible(const Vec2i& start, const Vec2i& end) const override
    {
        auto[xStart,yStart] = start;
        auto[xEnd,yEnd] = end;
        return yStart!=0 || xEnd!=0;
    }

    bool isYXRoutePossible(const Vec2i& start, const Vec2i& end) const override
    {
        auto[xStart,yStart] = start;
        auto[xEnd,yEnd] = end;
        return xStart!=0 || yEnd!=0;
    }
};

const NumericKeyboard kNumericKeyboard;
const DirectionalKeyboard kDirectionalKeyboard;
constexpr uint64_t kInf = std::numeric_limits<uint64_t>::max();

typedef std::tuple<char,char,int> Arguments;

struct ArgumentHasher
{
    size_t operator()(const Arguments& arguments) const
    {
        return (int)std::get<0>(arguments)*128*100 +
               (int)std::get<1>(arguments)*100 +
               (int)std::get<2>(arguments);
    }
};

struct Optimizer
{
    std::unordered_map<Arguments,uint64_t,ArgumentHasher> cache;

    uint64_t doTheWork(char currentChar, char targetChar, int level)
    {
        if (level == 0)
        {
            return 0;
        }

        auto& f = *this;

        const Keyboard* keyboard;
        if (level == kNumberOfRobots+1)
        {
            keyboard = &kNumericKeyboard;
        }
        else
        {
            keyboard = &kDirectionalKeyboard;
        }

        auto[currentX,currentY] = keyboard->find(currentChar);
        auto[targetX,targetY] = keyboard->find(targetChar);

        int dx = targetX - currentX;
        int dy = targetY - currentY;
        
        int mx = std::abs(dx);
        int my = std::abs(dy);

        char sx = dx>0? '>' : '<';
        char sy = dy>0? 'v' : '^';

        if (my == 0)
        {
            return f('A', sx, level-1) + mx + f(sx, 'A', level-1);
        }

        if (mx == 0)
        {
            return f('A', sy, level-1) + my + f(sy, 'A', level-1);
        }

        uint64_t option1 = kInf;
        uint64_t option2 = kInf;

        if (keyboard->isXYRoutePossible({currentX,currentY}, {targetX,targetY}))
        {
            option1 = f('A', sx, level-1) + mx + f(sx, sy, level-1) + my + f(sy, 'A', level-1);
        }

        if (keyboard->isYXRoutePossible({currentX,currentY}, {targetX,targetY}))
        {
            option2 = f('A', sy, level-1) + my + f(sy, sx, level-1) + mx + f(sx, 'A', level-1);
        }

        return std::min(option1, option2);

    }

    uint64_t operator()(char currentChar, char targetChar, int level = kNumberOfRobots+1)
    {
        Arguments arguments(currentChar, targetChar, level);
        auto it = cache.find(arguments);
        if (it == cache.end())
        {
            it = cache.emplace(arguments, doTheWork(currentChar, targetChar, level)).first;
        }
        return it->second;
    }

    uint64_t sequenceLength(const std::string& key)
    {
        uint64_t ans = 0;
        auto& f = *this;
        char hoveringOver = 'A';
        for (char c : key)
        {
            ans += f(hoveringOver, c) + 1;
            hoveringOver = c;
        }
        return ans;
    }

    uint64_t sequenceComplexity(const std::string& key)
    {
        return std::stoll(key.substr(0,key.length()-1))*sequenceLength(key);
    }
};

int main()
{
    uint64_t ans = 0;
    Optimizer optimizer;
    std::string key;
    while (std::cin >> key)
    {
        ans += optimizer.sequenceComplexity(key);
    }
    std::cout << ans << std::endl;
}
