#include <bits/stdc++.h>
using namespace std;

int main()
{
    int ans = 0;
    string line;
    while (getline(cin, line))
    {
        auto firstDigitIt = max_element(line.begin(), line.end()-1);
        auto secondDigitIt = max_element(firstDigitIt+1, line.end());
        int jolts = (*firstDigitIt-'0')*10 + (*secondDigitIt-'0');
        cout << line << endl;
        cout << jolts << endl;
        ans += jolts;
    }
    cout << ans << endl;
}
