#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

typedef vector<int> Report;

bool is_between(int x, int lo, int hi)
{
    return lo <= x && x <= hi;
}

template<class Iterator>
bool is_safe_aux(Iterator begin, Iterator end, Iterator removed)
{
    if (begin!=end && begin==removed)
    {
        ++begin;
    }
    if (begin == end)
    {
        return true;
    }
    bool ok = true;
    auto prev = begin;
    auto current = begin;
    while (++current != end && ok)
    {
        if (current == removed)
        {
            continue;
        }
        ok = is_between(*current-*prev, 1, 3);
        if (!ok && removed==end)
        {
            return is_safe_aux(begin, end, prev) || is_safe_aux(begin, end, current);
        }
        prev = current;
    }
    return ok;
};

bool is_safe(const Report& report)
{
    return is_safe_aux(report.begin(), report.end(), report.end()) || 
           is_safe_aux(report.rbegin(), report.rend(), report.rend());
}

int main() {
    int ans = 0;
    string line;
    while (getline(cin, line))
    {
        if (line.empty())
        {
            break;
        }
        istringstream iss(line);
        Report report;
        int x;
        while (iss >> x)
        {
            report.push_back(x);
        }
        ans += is_safe(report);
    }
    cout << ans << endl;
}
