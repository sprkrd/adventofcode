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
bool is_safe_aux(Iterator begin, Iterator end)
{
    if (begin == end)
    {
        return true;
    }
    int prev_value;
    do
    {
        prev_value = *begin;
        ++begin;
    } while (begin != end && is_between(*begin-prev_value, 1 , 3));
    return begin == end;
};

bool is_safe(const Report& report)
{
    return is_safe_aux(report.begin(), report.end()) || 
           is_safe_aux(report.rbegin(), report.rend());
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
