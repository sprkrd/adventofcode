#include <iostream>
#include <cstdint>

uint64_t calculateNextSecret(uint64_t currentSecret)
{
    constexpr uint64_t M = 1ULL << 24;
    uint64_t nextSecret = (currentSecret^(currentSecret<<6)) & (M-1);
    nextSecret = nextSecret^(nextSecret>>5);
    nextSecret = (nextSecret^(nextSecret<<11)) & (M-1);
    return nextSecret;
}

uint64_t fastForward(uint64_t secret, uint64_t iterations)
{
    for (uint64_t i = 0; i < iterations; ++i)
    {
        secret = calculateNextSecret(secret);
    }
    return secret;
}

int main()
{
    uint64_t ans = 0;
    uint64_t secret;
    while (std::cin >> secret)
    {
        ans += fastForward(secret, 2000);
    }
    std::cout << ans << std::endl;
}
