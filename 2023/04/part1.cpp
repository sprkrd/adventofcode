#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

struct Card {
    int id;
    vector<int> winning;
    vector<int> numbers;

    static Card parse(const string& line) {
        Card card;
        istringstream iss(line);
        string token;
        iss >> token;
        assert(token == "Card");
        iss >> token;
        card.id = stoi(token.substr(0, token.size()-1));
        while ((iss>>token) && token!="|") {
            card.winning.push_back(stoi(token));
        }
        while (iss>>token) {
            card.numbers.push_back(stoi(token));
        }
        return card;
    }

    uint64_t score() const {
        int good = 0;
        for (int n : numbers) {
            bool found = find(winning.begin(), winning.end(), n) != winning.end();
            good += found;
        }
        return good==0? 0 : (1ULL<<(good-1));
    }
};

ostream& operator<<(ostream& out, const Card& card) {
    out << "Card " << card.id << ':';
    for (int n : card.winning) {
        out << ' ' << n;
    }
    out << " |";
    for (int n : card.numbers) {
        out << ' ' << n;
    }
    return out;
}

int main() {
    uint64_t score_sum = 0;
    string line;
    while (getline(cin, line)) {
        auto card = Card::parse(line);
        score_sum += card.score();
    }
    cout << score_sum << endl;
}
