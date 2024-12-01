#include <algorithm>
#include <iostream>
using namespace std;


int main() {
    vector<int> left_list, right_list;
    int x, y;
    while (cin >> x >> y)
    {
        left_list.push_back(x);
        right_list.push_back(y);
    }
    sort(left_list.begin(), left_list.end());
    sort(right_list.begin(), right_list.end());
    int ans = 0;
    for (unsigned i = 0; i < left_list.size(); ++i)
    {
        ans += abs(left_list[i]-right_list[i]);
    }
    cout << ans << endl;
}
