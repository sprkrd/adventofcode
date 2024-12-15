#include <algorithm>
#include <array>
#include <bitset>
#include <iostream>
#include <string>
#include <vector>
#include <optional>

typedef std::pair<int, int> Vec2i;
typedef std::bitset<4> BorderFlags;

constexpr std::array<Vec2i, 4> kDirections{
    Vec2i(0, -1),
    Vec2i(1, 0),
    Vec2i(0, 1),
    Vec2i(-1, 0)
};

Vec2i operator+(const Vec2i& lhs, const Vec2i& rhs)
{
    return Vec2i(lhs.first + rhs.first, lhs.second + rhs.second);
}

struct Map
{
    std::string data;
    int height;
    int width;

    char& at(const Vec2i& position)
    {
        return data[position.second*width + position.first];
    }

    char at(const Vec2i& position) const
    {
        return data[position.second*width + position.first];
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
        while (getline(in, line))
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
                out << at({x, y});
            }
            out << '\n';
        }
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

std::istream& operator>>(std::istream& in, Map& matrix)
{
    matrix.read(in);
    return in;
}

std::ostream& operator<<(std::ostream& out, const Map& matrix)
{
    matrix.print(out);
    return out;
}

template<class T>
std::ostream& operator<<(std::ostream& out, const Matrix<T>& matrix)
{
    for (int y = 0; y < matrix.getHeight(); ++y)
    {
        for (int x = 0; x < matrix.getWidth(); ++x)
        {
            out << matrix.at({x, y}) << '\t';
        }
        out << '\n';
    }
    return out;
}

void labelConnectedComponent(const Map& map, Matrix<int>& labels, const Vec2i& seed, int label)
{
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
}

Matrix<int> labelConnectedComponents(const Map& map, int& numberOfComponents)
{
    numberOfComponents = 0;
    Matrix<int> labels(map.width, map.height, -1);
    for (int y = 0; y < map.height; ++y)
    {
        for (int x = 0; x < map.width; ++x)
        {
            Vec2i position(x, y);
            if (labels.at(position) == -1)
            {
                labelConnectedComponent(map, labels, position, numberOfComponents);
                ++numberOfComponents;
            }
        }
    }
    return labels;
}

BorderFlags getBorderFlags(const Matrix<int>& labels, const Vec2i& position)
{
    BorderFlags borders(0);
    int label = labels.at(position);
    for (int i = 0; i < 4; ++i)
    {
        auto neighbor = position + kDirections[i];
        if (!labels.validPosition(neighbor) || labels.at(neighbor) != label)
        {
            borders[i] = true;
        }
    }
    return borders;
}

std::vector<Vec2i> computeAreasAndSides(const Map& map)
{
    int numberOfComponents;
    auto labels = labelConnectedComponents(map, numberOfComponents);
    std::vector<Vec2i> output(numberOfComponents);
    Matrix<BorderFlags> borders(map.width, map.height, 0);
    for (int y = 0; y < map.height; ++y)
    {
        for (int x = 0; x < map.width; ++x)
        {
            Vec2i position(x,y);
            int label = labels.at(position);
            output[label].first++;
            BorderFlags borderFlags = getBorderFlags(labels, position);
            borders.at(position) = borderFlags;
            if (y > 0 && labels.at({x,y-1}) == label)
            {
                borderFlags &= ~borders.at({x,y-1});
            }
            if (x > 0 && labels.at({x-1,y}) == label)
            {
                borderFlags &= ~borders.at({x-1,y});
            }
            output[label].second += borderFlags.count();
        }
    }
    return output;
}

int calculateCost(const std::vector<Vec2i>& areasAndSides)
{
    int ans = 0;
    for (auto [area, sides] : areasAndSides)
    {
        ans += area*sides;
    }
    return ans;
}

int main()
{
    Map map;
    std::cin >> map;
    auto areasAndSides = computeAreasAndSides(map);
    int cost = calculateCost(areasAndSides);
    std::cout << cost << std::endl;
}

