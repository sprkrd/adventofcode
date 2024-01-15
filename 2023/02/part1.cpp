#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

struct Round {
    int r = 0, g = 0, b = 0;

    bool is_feasible(int r, int g, int b) const {
        return this->r<=r && this->g<=g && this->b<=b;
    }
};

struct Game {
    int id;
    vector<Round> rounds;

    static Game parse(const string& line) {
        Game game;

        istringstream iss(line);

        string token;
        iss >> token;
        assert(token == "Game");
        iss >> token;
        game.id = stoi(token.substr(0, token.size()-1));
        Round current_round;
        while (iss >> token) {
            int qty = stoi(token);
            iss >> token;
            if (token[0] == 'r') {
                current_round.r = qty;
            }
            else if (token[0] == 'g') {
                current_round.g = qty;
            }
            else if (token[0] == 'b') {
                current_round.b = qty;
            }
            else {
                assert(false);
            }
            if (token.back() == ';') {
                game.rounds.push_back(current_round);
                current_round = Round();
            }
        }
        game.rounds.push_back(current_round);
        return game;
    }

    bool is_feasible(int r, int g, int b) const {
        return all_of(rounds.begin(), rounds.end(),
                [=](const Round& round) {return round.is_feasible(r,g,b);} );
    }
};

ostream& operator<<(ostream& out, const Game& game) {
    out << "Game " << game.id << ": ";
    bool first_round = true;
    for (const auto& round : game.rounds) {
        if (!first_round) {
            out << "; ";
        }
        bool first_non_zero_count = true;
        if (round.r) {
            first_non_zero_count = false;
            out << round.r << " red";
        }
        if (round.g) {
            if (!first_non_zero_count) {
                out << ", ";
            }
            first_non_zero_count = false;
            out << round.g << " green";
        }
        if (round.b) {
            if (!first_non_zero_count) {
                out << ", ";
            }
            first_non_zero_count = false;
            out << round.b << " blue";
        }
        first_round = false;
    }
    return out;
}



int main() {
    int sum = 0;
    string line;
    while (getline(cin, line)) {
        auto game = Game::parse(line);
        sum += game.id*game.is_feasible(12,13,14);
    }
    cout << sum << endl;
}
