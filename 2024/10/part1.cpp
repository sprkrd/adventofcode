#include <algorithm>
#include <array>
#include <iostream>
#include <optional>
#include <vector>
#include <set>
#include <string>
using namespace std;

typedef pair<int, int> Vec2i;

constexpr array<Vec2i, 4> kDirections{
    Vec2i(0, -1),
    Vec2i(1, 0),
    Vec2i(0, 1),
    Vec2i(-1, 0)
};

Vec2i operator+(const Vec2i& lhs, const Vec2i& rhs)
{
    return Vec2i(lhs.first + rhs.first, lhs.second + rhs.second);
}

struct Matrix
{
    string data;
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

    optional<Vec2i> find(const string& chars, const Vec2i& offset = Vec2i(-1, 0)) const
    {
        size_t pos = data.find_first_of(chars, offset.second*width+offset.first+1);
        if (pos != string::npos)
        {
            return Vec2i(pos%width, pos/width);
        }
        return {};
    }

    int count(char c) const
    {
        return std::count(data.begin(), data.end(), c);
    }

    void read(istream& in)
    {
        data.clear();
        string line;
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

    void print(ostream& out) const
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

istream& operator>>(istream& in, Matrix& matrix)
{
    matrix.read(in);
    return in;
}

ostream& operator<<(ostream& out, const Matrix& matrix)
{
    matrix.print(out);
    return out;
}

int trailheadScore(const Matrix& matrix, const Vec2i& startPosition)
{
    set<Vec2i> visited;
    vector<Vec2i> stack{startPosition};
    int score = 0;
    while (!stack.empty())
    {
        auto position = stack.back();
        stack.pop_back();
        if (visited.count(position))
        {
            continue;
        }
        visited.insert(position);
        char value = matrix.at(position);
        if (value == '9')
        {
            ++score;
            continue;
        }
        for (const auto& dir : kDirections)
        {
            auto neighbor = position + dir;
            if (matrix.validPosition(neighbor) && matrix.at(neighbor) == value+1)
            {
                stack.push_back(neighbor);
            }
        }
    }
    return score;
}

int computeTrailheadScore(const Matrix& matrix)
{
    int score = 0;
    auto trailheadCandidate = matrix.find("0");
    while (trailheadCandidate)
    {
        score += trailheadScore(matrix, *trailheadCandidate);
        trailheadCandidate = matrix.find("0", *trailheadCandidate);

    }
    return score;
}

int main()
{
    Matrix matrix;
    cin >> matrix;
    cout << computeTrailheadScore(matrix) << endl;
}

