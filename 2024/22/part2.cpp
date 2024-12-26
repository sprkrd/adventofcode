#include <array>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

constexpr int kWindowSize = 4;
constexpr int kNumberOfIterations = 2000;
typedef std::array<int,kWindowSize> Window;

struct WindowHasher
{
    size_t operator()(const Window& window) const
    {
        size_t h = 0;
        for (int value : window)
        {
            h = h*19 + value + 9;
        }
        return h;
    }
};

typedef std::unordered_map<Window,int,WindowHasher> WindowMap;
typedef std::unordered_set<Window,WindowHasher> WindowRegister;

void slide(Window& window, int nextValue)
{
    for (size_t i = 0; i < window.size()-1; ++i)
    {
        window[i] = window[i+1];
    }
    window[window.size()-1] = nextValue;
}

uint64_t calculateNextSecret(uint64_t currentSecret)
{
    constexpr uint64_t M = 1ULL << 24;
    uint64_t nextSecret = (currentSecret^(currentSecret<<6)) & (M-1);
    nextSecret = nextSecret^(nextSecret>>5);
    nextSecret = (nextSecret^(nextSecret<<11)) & (M-1);
    return nextSecret;
}

void bookkeeping(uint64_t seed, WindowMap& windowScores)
{
    WindowRegister windowRegister;
    Window currentWindow{0};
    int previousLastDigit = seed%10;
    for (int i = 0; i < kNumberOfIterations; ++i)
    {
        seed = calculateNextSecret(seed);

        int lastDigit = seed%10;

        slide(currentWindow, lastDigit-previousLastDigit);

        if (i+1 >= kWindowSize && lastDigit != 0)
        {
            bool isNewWindow = windowRegister.insert(currentWindow).second;
            if (isNewWindow)
            {
                windowScores[currentWindow] += lastDigit;
            }
        }

        previousLastDigit = lastDigit;
    }
}

int main()
{
    WindowMap windowScores;

    uint64_t secret;
    while (std::cin >> secret)
    {
        bookkeeping(secret, windowScores);
    }

    Window bestWindow{0};
    int bestResult = 0;
    for (const auto&[window,windowScore] : windowScores)
    {
        if (windowScore > bestResult)
        {
            bestResult = windowScore;
            bestWindow = window;
        }
    }
    std::cout << "[" << bestWindow[0] << "," << bestWindow[1]
              << "," << bestWindow[1] << "," << bestWindow[2] << "]" << std::endl;
    std::cout << bestResult << std::endl;

}
