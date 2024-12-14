#include <iostream>
#include <unordered_map>
#include <string>

typedef unsigned long long Stone;
typedef std::unordered_map<Stone,size_t> StoneCollection;

void blink(Stone stone, size_t qty, StoneCollection& stoneCollection)
{
    if (stone == 0)
    {
        stoneCollection[1] += qty;
        return;
    }
    
    auto numberString = std::to_string(stone);
    if (numberString.length()%2 == 1)
    {
        stoneCollection[stone*2024] += qty;
    }
    else
    {
        Stone left = stoull(numberString.substr(0, numberString.length()/2));
        Stone right = stoi(numberString.substr(numberString.length()/2));
        stoneCollection[left] += qty;
        stoneCollection[right] += qty;
    }
}

void blink(StoneCollection& stoneCollection)
{
    StoneCollection nextStepCollection;
    for (auto[stone, qty] : stoneCollection)
    {
        blink(stone, qty, nextStepCollection);
    }
    stoneCollection = std::move(nextStepCollection);
}

size_t countStones(const StoneCollection& stoneCollection)
{
    size_t ans = 0;
    for (auto [_,qty] : stoneCollection)
    {
        ans += qty;
    }
    return ans;
}

int main()
{
    StoneCollection stoneCollection;
    Stone stone;
    while (std::cin >> stone)
    {
        stoneCollection[stone] += 1;
    }
    for (size_t i = 0; i < 75; ++i)
    {
        blink(stoneCollection);
    }
    std::cout << countStones(stoneCollection) << std::endl;
}
