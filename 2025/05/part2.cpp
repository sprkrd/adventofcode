#include <bits/stdc++.h>
using namespace std;

typedef uint64_t UInt;

struct Range
{
    UInt low;
    UInt high;
    
    Range(UInt low = 0, UInt high = 0) : low(low), high(high) {}
    
    bool Contains(UInt n) const
    {
        return low <= n && n <= high;
    }
    
    Range Merge(const Range& other) const
    {
        return Range(min(low,other.low), max(high,other.high));
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
        return high+1-low;
    }
};

void Process(vector<Range>& ranges)
{
    sort(ranges.begin(), ranges.end());
    vector<Range> simplified;
    simplified.push_back(ranges.front());
    for (size_t i = 1; i < ranges.size(); ++i)
    {
        if (simplified.back().Intersects(ranges[i]))
        {
            simplified.back() = simplified.back().Merge(ranges[i]);
        }
        else
        {
            simplified.push_back(ranges[i]);
        }
    }
    ranges = move(simplified);
}

bool Contain(const vector<Range>& ranges, UInt value)
{
    auto rangeIt = lower_bound(ranges.begin(), ranges.end(), value,
    [](const Range& range, UInt value_)
    {
        return range.high < value_;
    });
    if (rangeIt == ranges.end())
    {
        return false;
    }
    return rangeIt->Contains(value);
}

int main()
{
    vector<Range> ranges;
    vector<UInt> checkThese;
    string line;
    while (getline(cin,line) && !line.empty())
    {
        size_t dashIndex = line.find_first_of('-');
        UInt low = stoull(line.substr(0, dashIndex));
        UInt high = stoull(line.substr(dashIndex+1, line.size()-dashIndex-1));
        ranges.emplace_back(low, high);
    }
    Process(ranges);
    UInt ans = accumulate(ranges.begin(), ranges.end(), 0ULL,
    [](UInt acc, const Range& range)
    {
        return acc + range.Size();
    });
    cout << ans << endl;
}
