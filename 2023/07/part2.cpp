#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <tuple>
using namespace std;

typedef uint64_t Uint;

int card2index(char c) {
    switch (c) {
        case 'A':
            return 12;
        case 'K':
            return 11;
        case 'Q':
            return 10;
        case 'J':
            return 0;
        case 'T':
            return 9;
        default:
            return c-'0'-1;
    }
}

char index2card(int i) {
    static constexpr const char* cards = "23456789TJQKA";
    return cards[i];
}

enum class HandCategory {
    high_card,
    one_pair,
    two_pair,
    three_of_a_kind,
    full_house,
    four_of_a_kind,
    five_of_a_kind
};

HandCategory find_category(const string& cards) {
    vector<int> freq(13);
    for (char c : cards) {
        freq[card2index(c)] += 1;
    }

    size_t non_joker_most_freq = max_element(freq.begin()+1, freq.end()) -
        freq.begin();
    freq[non_joker_most_freq] += freq[0];
    freq[0] = 0;
    
    int num_pairs = 0;
    int num_triplets = 0;
    for (int f : freq) {
        num_pairs += f==2;
        num_triplets += f==3;
        if (f == 4) {
            return HandCategory::four_of_a_kind;
        }
        if (f == 5) {
            return HandCategory::five_of_a_kind;
        }
    }

    if (num_pairs==1 && num_triplets==1) {
        return HandCategory::full_house;
    }
    else if (num_triplets==1) {
        return HandCategory::three_of_a_kind;
    }
    else if (num_pairs == 2) {
        return HandCategory::two_pair;
    }
    else if (num_pairs ==  1) {
        return HandCategory::one_pair;
    }
    return HandCategory::high_card;
}

typedef tuple<Uint, Uint, string> Hand;

Uint hand_score(const string& cards) {
    typedef underlying_type_t<HandCategory> Num;
    HandCategory cat = find_category(cards);
    Uint score = static_cast<Num>(cat);
    for (char c : cards) {
        score = score*13 + card2index(c);
    }
    return score;
}

Hand read_hand(const string& line) {
    Hand hand;
    auto&[score, bid, cards] = hand;
    istringstream iss(line);
    iss >> cards >> bid;
    score = hand_score(cards);
    return hand;
}



int main() {
    vector<Hand> hands;
    string line;
    while (getline(cin, line)) {
        hands.push_back(read_hand(line));
    }
    Uint result = 0;
    sort(hands.begin(), hands.end());
    for (size_t i = 0; i < hands.size(); ++i) {
        result += (i+1)*get<1>(hands[i]);
    }
    cout << result << endl;

}
