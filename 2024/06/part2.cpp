#include <algorithm>
#include <array>
#include <iostream>
#include <optional>
#include <vector>
#include <string>
#include <unordered_set>
using namespace std;

typedef pair<int, int> Vec2i;

constexpr array<Vec2i, 4> kDirections{
    Vec2i(0, -1),
    Vec2i(1, 0),
    Vec2i(0, 1),
    Vec2i(-1, 0)
};

const string kDirectionChars = "^>v<";

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
        size_t pos = data.find_first_of(chars, 1+offset.second*width+offset.first);
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

struct State
{
    const Matrix& matrix;
    Vec2i position;
    int direction;

    State(const Matrix& matrix) :
        matrix(matrix),
        position(matrix.find("^").value()),
        direction(0)
    {
    }

    bool step()
    {
        bool done = false;

        Vec2i nextPosition = position + kDirections[direction];
        if (!matrix.validPosition(nextPosition))
        {
            done = true;
        }
        else if (matrix.at(nextPosition) == '#')
        {
            direction = (direction+1)%kDirections.size();
        }
        else
        {
            position = nextPosition;
        }

        return done;
    }

    bool operator==(const State& other) const
    {
        return position == other.position && direction == other.direction;
    }
};

namespace std
{

template<>
struct hash<State>
{
    size_t operator()(const State& state) const
    {
        return 4*(state.position.first + state.position.second*state.matrix.width) +
               state.direction;
    }
};

}

int main()
{
    Matrix matrix;
    cin >> matrix;

    State initialState(matrix);

    // mark all positions visited by guard
    {
        State currentState = initialState;
        matrix.at(currentState.position) = 'X';
        while (!currentState.step())
        {
            matrix.at(currentState.position) = 'X';
        }
    }

    int ans = 0;

    optional<Vec2i> position = matrix.find("X");
    while (position)
    {
        if (*position != initialState.position)
        {
            matrix.at(*position) = '#';

            // simulate guard path
            unordered_set<State> visited{initialState};
            State currentState(initialState);
            bool outOfMap = false;
            while (!(outOfMap=currentState.step()) && !visited.count(currentState))
            {
                visited.insert(currentState);
            }

            ans += !outOfMap; 
            matrix.at(*position) = 'X';
        }
        position = matrix.find("X", *position);
    }

    cout << ans << endl;
}

