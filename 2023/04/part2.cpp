#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

struct Card {
    int id;
    int qty;
    vector<int> winning;
    vector<int> numbers;

    static Card parse(const string& line) {
        Card card;
        card.qty = 1;
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
        uint64_t good = 0;
        for (int n : numbers) {
            bool found = find(winning.begin(), winning.end(), n) != winning.end();
            good += found;
        }
        return good;
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
    string line;

    vector<Card> cards;

    while (getline(cin, line)) {
        auto card = Card::parse(line);
        cards.push_back(card);
    }

    uint64_t scratch_cards = 0;
    for (size_t i = 0; i < cards.size(); ++i) {
        scratch_cards += cards[i].qty;
        uint64_t score_i = cards[i].score();
        for (size_t j = i+1; j < i+score_i+1; ++j) {
            cards[j].qty += cards[i].qty;
        }
    }

    cout << scratch_cards << endl;
}
