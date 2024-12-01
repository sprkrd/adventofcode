#include <algorithm>
#include <iostream>
#include <unordered_set>
using namespace std;


int main() {
    vector<int> left_list;
    unordered_multiset<int> id_count;
    int x, y;
    while (cin >> x >> y)
    {
        left_list.push_back(x);
        id_count.insert(y);
    }
    int ans = 0;
    for (int x : left_list)
    {
        ans += x*id_count.count(x);
    }
    cout << ans << endl;
}
