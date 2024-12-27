#include <array>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

typedef std::array<int,5> Key;
typedef std::array<int,5> Lock;

bool keyFits(const Key& key, const Lock& lock)
{
    for (int i = 0; i < 5; ++i)
    {
        if (key[i] + lock[i] > 5)
        {
            return false;
        }
    }
    return true;
}

int main()
{
    std::vector<Key> keys;
    std::vector<Lock> locks;
    std::string line;
    while (std::getline(std::cin, line))
    {
        
        if (line[0] == '.')
        {
            Key key{5,5,5,5,5};
            for (int i = 0; i < 6; ++i)
            {
                std::getline(std::cin, line);
                for (int j = 0; j < 5; ++j)
                {
                    key[j] -= line[j] == '.';
                }
            }
            // std::cout << "key: " << key[0] << "," << key[1] << "," << key[2] << "," << key[3] << "," << key[4] << std::endl;
            keys.push_back(key);
        }
        else
        {
            Lock lock{0};
            for (int i = 0; i < 6; ++i)
            {
                std::getline(std::cin, line);
                for (int j = 0; j < 5; ++j)
                {
                    lock[j] += line[j] == '#';
                }
            }
            // std::cout << "lock: " << lock[0] << "," << lock[1] << "," << lock[2] << "," << lock[3] << "," << lock[4] << std::endl;
            locks.push_back(lock);
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    int ans = 0;
    for (const auto& key : keys)
    {
        for (const auto& lock : locks)
        {
            ans += keyFits(key, lock);
        }
    }
    std::cout << ans << std::endl;
}
