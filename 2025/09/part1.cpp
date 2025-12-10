#include <bits/stdc++.h>
using namespace std;

typedef int64_t Int;
typedef uint64_t UInt;
typedef array<Int,2> Point2i;

vector<string> Split(const string& str, const string& delim=" \n\t") {
    vector<string> parts;
    size_t pos = 0;
    while (pos < str.length())
    {
        size_t pos_next = str.find_first_of(delim, pos);
        if (pos_next > pos)
        {
            parts.push_back(str.substr(pos, pos_next-pos));
        }
        pos = pos_next == string::npos? string::npos : pos_next + 1;
    }
    return parts;
}

vector<Point2i> Input()
{
    vector<Point2i> input;
    string line;
    while (getline(cin,line))
    {
        auto parts = Split(line, ",");
        Int x = stoll(parts[0]);
        Int y = stoll(parts[1]);
        input.push_back(Point2i{x,y});
    }
    return input;
}

Int RectangleArea(const Point2i& p1, const Point2i& p2)
{
    Int dx = abs(p2[0]-p1[0]) + 1;
    Int dy = abs(p2[1]-p1[1]) + 1;
    return dx*dy;
}

ostream& operator<<(ostream& out, const Point2i& point)
{
    return out << '(' << point[0] << ',' << point[1] << ')';
}

int main()
{
    auto input = Input();
    
    Int maxArea = 0;
    for (size_t i = 0; i < input.size(); ++i)
    {
        for (size_t j = i + 1; j < input.size(); ++j)
        {
            maxArea = max(RectangleArea(input[i], input[j]), maxArea);
        }
    }
    cout << maxArea << endl;
}
