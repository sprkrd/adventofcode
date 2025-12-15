#include <bits/stdc++.h>
#include "netpbm.hpp"
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
    
    Range& MergeUpdate(const Range& other)
    {
        low = min(low,other.low);
        high = max(high,other.high);
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
};

struct Rectangle
{
    Point2i corner1;
    Point2i corner2;
    
    Int Area() const
    {
        Int dx = abs(corner2[0]-corner1[0]) + 1;
        Int dy = abs(corner2[1]-corner1[1]) + 1;
        return dx*dy;
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
    
        class IObserver
        {
            public:
            
                virtual void Notify(const Sweeper& sweeper) = 0;
            
                virtual ~IObserver() = default;
        };
        
        Int GetCurrentX() const
        {
            return _i < _points.size()? _points[_i][0] : _points.back()[0];
        }
        
        const vector<Range<Int>>& GetCurrentFront() const
        {
            return _activeFront;
        }
        
        vector<Point2i> GetActivePoints() const
        {
            vector<Point2i> activePoints;
            for (const auto& point : _activePoints)
            {
                activePoints.push_back(_points[point.pointIndex]);
            }
            return activePoints;
        }
    
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
        
        const Rectangle& GetLargestRectangle() const
        {
            return _largestRectangle;
        }
        
        Int GetMaxArea() const
        {
            return _maxArea;
        }
        
        void AddObserver(IObserver& observer)
        {
            _observers.push_back(&observer);
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
            
            for (auto* observer : _observers)
            {
                observer->Notify(*this);
            }
            
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
                        Rectangle rectangle = {point, _points[other.pointIndex]};
                        Int area = rectangle.Area();
                        if (area > _maxArea)
                        {
                            _maxArea = area;
                            _largestRectangle = rectangle;
                        }
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
        
        vector<IObserver*> _observers;
        vector<Point2i> _points;
        vector<Range<Int>> _activeFront;
        vector<ActivePoint> _activePoints;
        Rectangle _largestRectangle;
        Int _maxArea;
        size_t _i;
};

const ash::Pixel kGreenTile = {150,176,8};
const ash::Pixel kRedTile = {228,8,10};
const ash::Pixel kFrontierColor = {204,108,231};

class ImageRecorder : public Sweeper::IObserver
{
    public:
    
        ImageRecorder(string pathPrefix, size_t frameWidth, size_t frameHeight)
        : _largestRectangle{0,0}
        , _pathPrefix(move(pathPrefix))
        , _background(frameWidth, frameHeight)
        , _frame(frameWidth, frameHeight)
        , _frameIdx(0)
        {
        }
        
        void Init(vector<Point2i> points)
        {
            _points = move(points);
            _x0 = _points[0][0];
            _y0 = _points[0][1];
            Int _xmax = _x0;
            Int _ymax = _y0;
            for (auto[x,y] : _points)
            {
                _x0 = min(x, _x0);
                _y0 = min(y, _y0);
                _xmax = max(x, _xmax);
                _ymax = max(y, _ymax);
            }
            _scaleFactor = min((_frame.Width()-1)/double(_xmax-_x0), (_frame.Height()-1)/double(_ymax-_y0));
        }
    
        void Notify(const Sweeper& sweeper) override
        {
            Int nextX = sweeper.GetCurrentX();
            for (const auto& front : _currentFront)
            {
                auto begin = WorldToFrameCoords(Point2i{_currentX, front.low});
                auto end = WorldToFrameCoords(Point2i{nextX, front.high});
                ash::FillRectangle(_background, begin[0], begin[1], end[0], end[1], kGreenTile);
            }
            _currentX = nextX;
            _currentFront = sweeper.GetCurrentFront();
            _largestRectangle = sweeper.GetLargestRectangle();
            _visiblePoints = sweeper.GetActivePoints();
            Draw();
        }
        
        void Draw()
        {
            ostringstream filename;
            filename << _pathPrefix << setw(3) << setfill('0') << _frameIdx << ".ppm"; 
            
            _frame = _background;
          
            for (size_t i = 0; i < _points.size(); ++i)
            {
                const auto& prev = WorldToFrameCoords(i==0? _points.back() : _points[i-1]);
                const auto& point = WorldToFrameCoords(_points[i]);
                ash::DrawSegment(_frame, prev[0], prev[1], point[0], point[1], kGreenTile);
            }
            
            if (_frameIdx > 0)
            {
                auto begin = WorldToFrameCoords(_largestRectangle.corner1);
                auto end = WorldToFrameCoords(_largestRectangle.corner2);
                ash::FillRectangle(_frame, begin[0], begin[1], end[0], end[1], kRedTile);
            }
            
            for (const auto& point : _visiblePoints)
            {
                const auto& pointImg = WorldToFrameCoords(point);
                ash::DrawCross(_frame, pointImg[0], pointImg[1], 3, kRedTile);
            }
            
            for (const auto& front : _currentFront)
            {
                auto begin = WorldToFrameCoords(Point2i{_currentX, front.low});
                auto end = WorldToFrameCoords(Point2i{_currentX, front.high});
                ash::DrawSegment(_frame, begin[0], begin[1], end[0], end[1], kFrontierColor);
            }
            
            
            _frame.Save(filename.str());
            ++_frameIdx;
        }
        
    private:
    
        Point2i WorldToFrameCoords(const Point2i& point)
        {
            return Point2i{(Int)round((point[0]-_x0)*_scaleFactor), (Int)round((point[1]-_y0)*_scaleFactor)};
        }
        
        double _scaleFactor;
        Int _x0;
        Int _y0;
        Int _currentX;
        vector<Point2i> _points;
        vector<Point2i> _visiblePoints;
        vector<Range<Int>> _currentFront;
        Rectangle _largestRectangle;
        string _pathPrefix;
        ash::Image _background;
        ash::Image _frame;
        size_t _frameIdx;
};

int main(int argc, char* argv[])
{
    string outPrefix = "frame_";
    int outWidth = 640;
    int outHeight = 480;
    if (argc >= 2) outPrefix = argv[1];
    if (argc >= 3) outWidth = stoi(argv[2]);
    if (argc >= 4) outHeight = stoi(argv[3]);
    auto input = Input();
    ImageRecorder imageRecorder(outPrefix, outWidth, outHeight);
    imageRecorder.Init(input);
    Sweeper sweeper(move(input));
    sweeper.AddObserver(imageRecorder);
    sweeper.SweepUntilEnd();
    cout << sweeper.GetMaxArea() << endl;
}
