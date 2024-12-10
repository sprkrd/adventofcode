#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

typedef pair<int,int> Vec2i;

Vec2i operator-(const Vec2i& left, const Vec2i& right)
{
    return Vec2i(left.first - right.first, left.second - right.second);
}

Vec2i operator+(const Vec2i& left, const Vec2i& right)
{
    return Vec2i(left.first + right.first, left.second + right.second);
}

Vec2i operator*(int scalar, const Vec2i& vec)
{
    return Vec2i(scalar*vec.first, scalar*vec.second);
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

void markAntinodes(Matrix& matrix, const vector<Vec2i>& antennae)
{
    for (unsigned i = 0; i < antennae.size(); ++i)
    {
        for (unsigned j = i + 1; j < antennae.size(); ++j)
        {
            Vec2i antinode1 = 2*antennae[j] - antennae[i];
            if (matrix.validPosition(antinode1))
            {
                matrix.at(antinode1) = '#';
            }
            Vec2i antinode2 = 2*antennae[i] - antennae[j];
            if (matrix.validPosition(antinode2))
            {
                matrix.at(antinode2) = '#';
            }
        }
    }
}

int main()
{
    Matrix matrix;
    cin >> matrix;
    
    vector<vector<Vec2i>> antennae(128);
    
    for (int y = 0; y < matrix.height; ++y)
    {
        for (int x = 0; x < matrix.width; ++x)
        {
            char cellValue = matrix.at(x, y);
            if (cellValue != '.')
            {
                antennae[int(cellValue)].emplace_back(x, y);
            }
        }
    }
    for (int i = 0; i < 128; ++i)
    {
        markAntinodes(matrix, antennae[i]);
    }
    
    cout << matrix.count('#') << endl;
}
