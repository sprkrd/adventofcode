#include <iostream>
#include <string>
#include <vector>
using namespace std;

vector<string> split(const string& str, const string& delim=" \n\t") {
    vector<string> parts;
    size_t pos = 0;
    while (pos < str.length()) {
        size_t pos_next = str.find_first_of(delim, pos);
        if (pos_next > pos)
            parts.push_back(str.substr(pos, pos_next-pos));
        pos = pos_next == string::npos? string::npos : pos_next + 1;
    }
    return parts;
}

struct Player {
    int position;
    int score;
};

struct State {
    Player players[2];
    int turn;
    int die_state;
};

ostream& operator<<(ostream& out, const State& state) {
    out << "State {\n";
    out << "  turn: " << state.turn << ",\n"
        << "  die_state: " << state.die_state << ",\n"
        << "  players: [\n"
        << "    Player {\n"
        << "      position: " << state.players[0].position << ",\n"
        << "      score: " << state.players[0].score << '\n'
        << "    },\n"
        << "    Player {\n"
        << "      position: " << state.players[1].position << ",\n"
        << "      score: " << state.players[1].score << '\n'
        << "    }\n"
        << "  ]\n";
    out << '}';
    return out;
}

void step(State& state) {
    Player& current_player = state.players[state.turn % 2];
    int throw_result = 3*(state.die_state + 1) - 100*max(0, state.die_state- 98);
    state.die_state += 3 - 100*(state.die_state >= 98);
    current_player.position = (current_player.position-1 + throw_result)%10 + 1;
    current_player.score += current_player.position;
    ++state.turn;
}

int main() {
    State state;
    state.players[0].score = state.players[1].score = 0;
    state.turn = 0;
    state.die_state = 1;
    string line1, line2;
    getline(cin, line1);
    getline(cin, line2);
    auto parts1 = split(line1);
    auto parts2 = split(line2);
    state.players[0].position = stoi(parts1.back());
    state.players[1].position = stoi(parts2.back());

    while (state.players[0].score < 1000 && state.players[1].score < 1000) {
        step(state);
    }

    int num_die_throws = 3*state.turn;
    int score_loser = min(state.players[0].score, state.players[1].score);
    int ans = num_die_throws*score_loser;

    cout << ans << endl;
}
