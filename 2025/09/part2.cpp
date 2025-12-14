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
    
    Range CreateEmptyRange()
    {
        return Range(1, 0);
    }
    
    Range(T low = 0, T high = 0) : low(low), high(high) {}
    
    bool Contains(T n) const
    {
        return low <= n && n <= high;
    }
    
    Range& MergeUpdate(const Range& other)
    {
        if (Empty())
        {
            low = other.low;
            high = other.high;
        }
        else
        {
            low = min(low,other.low);
            high = max(high,other.high);
        }
        return *this;
    }
    
    bool IsAdjacent(const Range& other) const
    {
        return low == other.high || high == other.low;
    }
    
    bool Intersects(const Range& other) const
    {
        return Contains(other.low) || other.Contains(low);
    }
    
    Range Intersection(const Range& other) const
    {
        return Range(max(low,other.low), min(high,other.high));
    }
    
    bool Empty() const
    {
        return Size() == 0;
    }
    
    UInt Size() const
    {
        return max(Int(0),high+1-low);
    }
};

template<class T>
ostream& operator<<(ostream& out, const Range<T>& range)
{
    return out << '[' << range.low << ',' << range.high << ']';
}

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

void PushRange(vector<Range<Int>>& ranges, const Range<Int>& newRange)
{
    if (ranges.empty() || !newRange.Intersects(ranges.back()))
    {
        ranges.push_back(newRange);
    }
    else
    {
        ranges.back().MergeUpdate(newRange);
    }
}

class Sweeper
{
    public:
        Sweeper(vector<Point2i> points) : _maxArea(0), _i(0)
        {
            _points = move(points);
            sort(_points.begin(), _points.end());
        }
        
        void SweepUntilEnd()
        {
            while (_i < _points.size())
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
            size_t j = _i+2;
            while (j < _points.size() && _points[j][0] == _points[_i][0])
            {
                j += 2;
            }
            
            UpdateActiveFront(j);
            AddNewVisiblePoints(j);
            UpdateVisibility();
            
            _i = j;
        }
        
        void UpdateActiveFront(size_t j)
        {
            vector<Range<Int>> newActiveFront;
            size_t k = 0;
            for (size_t i = _i; i < j; i += 2)
            {
                const auto& head = _points[i];
                const auto& tail = _points[i+1];
                
                Int yLow = head[1];
                Int yHigh = tail[1];
                Range yRange(yLow, yHigh);
                
                while (k < _activeFront.size() && _activeFront[k].high < yLow)
                {
                    PushRange(newActiveFront, _activeFront[k]);
                    ++k;
                }
                
                if (k == _activeFront.size() || yHigh < _activeFront[k].low)
                {
                    PushRange(newActiveFront, yRange);
                }
                else if (yRange.IsAdjacent(_activeFront[k]))
                {
                    _activeFront[k].MergeUpdate(yRange);
                }
                else if (yLow == _activeFront[k].low && yHigh < _activeFront[k].high)
                {
                    _activeFront[k].low = yHigh;
                }
                else if (yHigh == _activeFront[k].high && yLow > _activeFront[k].low)
                {
                    _activeFront[k].high = yLow;
                }
                else if (yLow > _activeFront[k].low && yHigh < _activeFront[k].high)
                {
                    PushRange(newActiveFront, Range(_activeFront[k].low, yLow));
                    _activeFront[k].low = yHigh;
                }
                else
                {
                    ++k;
                }
            }
            
            while (k < _activeFront.size())
            {
                PushRange(newActiveFront, _activeFront[k]);
                ++k;
            }
            
            _activeFront = move(newActiveFront);
        }
        
        optional<Range<Int>> FindFront(Int y)
        {
            for (const auto& front : _activeFront)
            {
                if (front.Contains(y))
                {
                    return front;
                }
            }
            return {};
        }
        
        void UpdateVisibility()
        {
            vector<ActivePoint> newActivePoints;
            for (const auto& activePoint : _activePoints)
            {
                const auto& point = _points[activePoint.pointIndex];
                if (auto front = FindFront(point[1]))
                {
                    auto updatedVisibility = activePoint.visibility.Intersection(*front);
                    newActivePoints.push_back({activePoint.pointIndex, updatedVisibility});
                }
            }
            _activePoints = move(newActivePoints);
        }
        
        void AddNewVisiblePoints(size_t j)
        {
            for (size_t i = _i; i < j; ++i)
            {
                const auto& point = _points[i];
                for (const auto& other : _activePoints)
                {
                    if (other.visibility.Contains(point[1]))
                    {
                        _maxArea = max(_maxArea, RectangleArea(point, _points[other.pointIndex]));
                    }
                }
                if (auto visibility = FindFront(point[1]))
                {
                    _activePoints.push_back({i, *visibility});
                }
            }
        }
        
        struct ActivePoint
        {
            size_t pointIndex;
            Range<Int> visibility;
        };
        
        vector<Point2i> _points;
        vector<Range<Int>> _activeFront;
        vector<ActivePoint> _activePoints;
        Int _maxArea;
        size_t _i;
};

int main()
{
    Sweeper sweeper(Input());
    sweeper.SweepUntilEnd();
    cout << sweeper.GetMaxArea() << endl;
}
