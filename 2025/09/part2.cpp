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

template<class T>
struct Range
{
    T low;
    T high;
    
    Range(T low = 0, T high = 0) : low(low), high(high) {}
    
    bool Contains(T n) const
    {
        return low <= n && n <= high;
    }
    
    bool Contains(const Range& other) const
    {
        return low <= other.low && other.high <= high;
    }
    
    Range Merge(const Range& other) const
    {
        return Range(min(low,other.low), max(high,other.high));
    }
    
    Range Intersection(const Range& other) const
    {
        return Range(max(low,other.low), min(high,other.high));
    }
    
    bool Intersects(const Range& other) const
    {
        return Contains(other.low) || other.Contains(low);
    }
    
    bool operator<(const Range& other) const
    {
        if (low != other.low) return low < other.low;
        return high < other.high;
    }
    
    UInt Size() const
    {
        return max(Int(0),high+1-low);
    }
};

ostream& operator<<(ostream& out, const Point2i& point)
{
    return out << '(' << point[0] << ',' << point[1] << ')';
}

Int RectangleArea(const Point2i& p1, const Point2i& p2)
{
    Int dx = abs(p2[0]-p1[0]) + 1;
    Int dy = abs(p2[1]-p1[1]) + 1;
    return dx*dy;
}

struct VerticalSegment
{
    Int x;
    Range<Int> yRange;
    
    VerticalSegment(const Point2i& begin, const Point2i& end)
    : x(begin[0])
    , yRange(min(begin[1],end[1]), max(begin[1],end[1]))
    {}
    
    bool operator<(const VerticalSegment& other) const
    {
        if (x!=other.x) return x<other.x;
        return yRange<other.yRange;
    }jk
};


ostream& operator<<(ostream& out, const Segment& segment)
{
    return out << segment.x << ",[" << yRange.low << ',' << yRange.high << ']';
}

class Sweeper
{
    public:
        Sweeper(const vector<Point2i>& input) : _maxArea(0), _i(0)
        {
            for (size_t i = 0; i < input.size(); ++i)
            {
                auto next = i==input.size()-1? input.front() : input[i+1];
                if (input[i][0]==next[0])
                {
                    _verticalSegments.emplace_back(input[i], next);
                }
            }
            sort(_verticalSegments.begin(), _verticalSegments.end());
            _closesShape.resize(_verticalSegments.size());
        }
        
        void SweepUntilEnd()
        {
            while (_i < _verticalSegments.size())
            {
                Advance();
            }
        }
        
        Int GetMaxArea() const
        {
            return _maxArea;
        }
        
    private:
        void Advance()
        {
            const auto& nextSegment = _verticalSegments[_i];
            bool intersectsActiveSet = false;
            for (int j = _i-1; j >= 0; --j)
            {
                const auto& segment = _verticalSegments[j];
                
                if (segment.yRange.Intersection(nextSegment.yRange).Size() > 1) continue;
                
                intersectsActiveSet = true;
                
            }
            for (const auto& verticalSegment : _activeSet)
            {
                if (verticalSegment.yRange.Intersection(nextSegment.yRange).Size() <= 1)
                {
                    nextActiveSet.push_back(verticalSegment);
                    continue;
                }
                intersectsActiveSet = true;
                
            }
            if (!intersectsActiveSet)
            {
                nextActiveSet.push_back(nextSegment);
                UpdateArea(nextSegment);
            }
            _activeSet = move(nextActiveSet);
            ++_i;
            cout << "-> ";
            for (const auto& segment : _activeSet)
            {
                cout << segment << "; ";
            }
            cout << endl;
        }
        
        void UpdateArea(const VerticalSegment& nextSegment)
        {
            for (const auto& segment : _activeSet)
            {
                if (segment.redTop&&segment.redBottom&&segment.)
                {
                    _maxArea = max(_maxArea, RectangleArea(segment.Top(), nextSegment.Bottom()));
                    cout << segment << " and " << nextSegment << ": " << _maxArea << endl;
                }
                
                if (segment.redBottom&&nextSegment.redTop)
                {
                    _maxArea = max(_maxArea, RectangleArea(segment.Bottom(), nextSegment.Top()));
                }
                
                if (segment.redTop&&nextSegment.redTop)
                {
                    _maxArea = max(_maxArea, RectangleArea(segment.Top(), nextSegment.Top()));
                }
                
                if (segment.redBottom&&nextSegment.redBottom)
                {
                    _maxArea = max(_maxArea, RectangleArea(segment.Bottom(), nextSegment.Bottom()));
                }
            }
        }
    
        vector<VerticalSegment> _verticalSegments;
        vector<Point2i>
        vector<bool> _closesShape;
        Int _maxArea;
        size_t _i;
};

int main()
{
    auto input = Input();
    Sweeper sweeper(input);
    sweeper.SweepUntilEnd();
    cout << sweeper.GetMaxArea() << endl;
}
