#include <iostream>
#include <string>
#include <list>

typedef unsigned long long Stone;
typedef std::list<Stone> StoneList;

void update(StoneList& stoneList, StoneList::iterator it)
{
    Stone number = *it;
    
    if (number == 0)
    {
        *it = 1;
        return;
    }
    
    auto numberString = std::to_string(number);
    if (numberString.length()%2 == 1)
    {
        *it *= 2024;
    }
    else
    {
        stoneList.insert(it, stoull(numberString.substr(0, numberString.length()/2)));
        *it = stoi(numberString.substr(numberString.length()/2));
    }
}

void update(StoneList& stoneList)
{
    for (auto it = stoneList.begin(); it != stoneList.end(); ++it)
    {
        update(stoneList, it);
    }
}

void display(const StoneList& stoneList, std::ostream& out)
{
    for (auto number : stoneList)
    {
        out << number << ' ';
    }
    out << '\n';
}

int main()
{
    StoneList stoneList;
    int n;
    while (std::cin >> n)
    {
        stoneList.push_back(n);
    }
    for (int i = 0; i < 25; ++i)
    {
        update(stoneList);
    }
    std::cout << stoneList.size() << std::endl;
}
