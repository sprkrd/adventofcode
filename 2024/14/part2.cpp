#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <vector>

#include <ncurses.h>

typedef std::pair<int,int> Vec2i;

//constexpr Vec2i kMapDimensions(11, 7);
constexpr Vec2i kMapDimensions(101, 103);

constexpr std::array<Vec2i, 4> kDirections{
    Vec2i(0, -1),
    Vec2i(1, 0),
    Vec2i(0, 1),
    Vec2i(-1, 0)
};

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

struct Map
{
    std::string data;
    int width;
    int height;


    Map(int width, int height, char defaultChar='.') :
        data(width*height, defaultChar),
        width(width),
        height(height)
    {
    }

    bool validPosition(const Vec2i& position) const
    {
        return position.first >= 0 && position.first < width &&
               position.second >= 0 && position.second < height;
    }

    char& at(const Vec2i& position)
    {
        return data[position.second*width + position.first];
    }

    char at(const Vec2i& position) const
    {
        return data[position.second*width + position.first];
    }

    void fill(char fillChar)
    {
        std::fill(data.begin(), data.end(), fillChar);
    }
};

template<class T>
class Matrix
{
    public:

        Matrix(int width, int height, T defaultValue = T()) :
            width(width),
            height(height),
            data(width*height, defaultValue)
        {
        }

        int getWidth() const
        {
            return width;
        }

        int getHeight() const
        {
            return height;
        }

        bool validPosition(const Vec2i& position) const
        {
            return 0 <= position.first && position.first < width &&
                   0 <= position.second && position.second < height;
        }

        T& at(const Vec2i& position)
        {
            return data[position.second*width + position.first];
        }

        T at(const Vec2i& position) const
        {
            return data[position.second*width + position.first];
        }

    private:
        int width;
        int height;
        std::vector<T> data;
};

int labelConnectedComponent(const Map& map, Matrix<int>& labels, const Vec2i& seed, int label)
{
    int size = 0;
    char type = map.at(seed);
    std::vector<Vec2i> stack{seed};
    while (!stack.empty())
    {
        Vec2i position = stack.back();
        stack.pop_back();
        if (labels.at(position) != -1)
        {
            continue;
        }
        ++size;
        labels.at(position) = label;
        for (Vec2i direction : kDirections)
        {
            Vec2i neighbor = position + direction;
            if (map.validPosition(neighbor) && map.at(neighbor) == type)
            {
            Vec2i neighbor = position + direction;
                stack.push_back(neighbor);
            }
        }
    }
    return size;
}

int largestComponentSize(const Map& map)
{
    int largestSize = 0;
    int numberOfComponents = 0;
    Matrix<int> labels(map.width, map.height, -1);
    for (int y = 0; y < map.height; ++y)
    {
        for (int x = 0; x < map.width; ++x)
        {
            Vec2i position(x, y);
            if (map.at(position) == '#' && labels.at(position) == -1)
            {
                int size = labelConnectedComponent(map, labels, position, numberOfComponents);
                ++numberOfComponents;
                largestSize = std::max(largestSize, size);
            }
        }
    }
    return largestSize;
}

class Application
{
    public:

        Application() : map(kMapDimensions.first, kMapDimensions.second, '.')
        {
        }

        void initialize()
        {
            std::fstream in("input", std::ios_base::in);
            std::string line;
            while (std::getline(in, line))
            {
                robotStates.push_back(parseRobotState(line));
            }
            nticks = 0;
            initscr();
            noecho();
            timeout(5);
        }

        int run()
        {
            updateScreen();
            bool exit = false;
            bool pause = false;
            while (!exit)
            {
                int keyPressed = getch();
                if (keyPressed == 'q')
                {
                    exit = true;
                }
                else if (keyPressed == 'p')
                {
                    pause = !pause;
                }
                if (!pause)
                {
                    step();
                    updateScreen();

                    int componentSize = largestComponentSize(map);
                    if (componentSize >= 20)
                    {
                        pause = true;
                        move(0,0);
                    }
                }
            }
            return 0;
        }

        void step()
        {
            ++nticks;
            map.fill(' ');
            for (auto& robotState : robotStates)
            {
                robotState.update(1);
                map.at(robotState.position) = '#';
            }
        }

        void updateScreen()
        {
            clear();
            printw("#nticks: %d", nticks);
            for (int y = 0; y < map.height; ++y)
            {
                move(y+1, 0);
                for (int x = 0; x < map.width; ++x)
                {
                    addch(map.at({x,y}));
                }
            }
            refresh();
        }

        ~Application()
        {
            endwin();
        }

    private:

        int nticks;
        std::vector<RobotState> robotStates;
        Map map;
};



int main()
{
    Application app;
    app.initialize();
    app.run();
}
