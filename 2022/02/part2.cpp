#include <iostream>
#include <map>
using namespace std;

int rock = 1;
int paper = 2;
int scissors = 3;

int round_score(int opponent, int me) {
    typedef pair<int,int> Point2i;
    static map<Point2i,int> lu {
        {{rock,rock}, 3},
        {{rock,paper}, 6},
        {{rock,scissors},0},
        {{paper,rock},0},
        {{paper,paper},3},
        {{paper,scissors},6},
        {{scissors,rock},6},
        {{scissors,paper},0},
        {{scissors,scissors},3}
    };
    return me + lu[Point2i(opponent,me)];
}

int main() {
    int score = 0;
    char opponent, me;
    while (cin >> opponent >> me) {
        score += round_score(opponent-'A'+1, me-'X'+1);
    }
    cout << score << endl;
}
