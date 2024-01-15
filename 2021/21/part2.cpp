#include <cstdint>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
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

    bool operator==(const Player& other) const {
        return position == other.position && score == other.score;
    }
};

struct State {
    Player players[2];
    int turn;

    bool operator==(const State& other) const {
        return players[0] == other.players[0] &&
               players[1] == other.players[1] &&
               turn == other.turn;
    }
};

template<class T>
void hash_combine(size_t& seed, const T& value) {
    seed ^= hash<T>{}(value) + 0x9E3779B97F4A7C15 + (seed<<12) + (seed>>4);
}

namespace std {
    template<> struct hash<Player> {
        size_t operator()(const Player& player) const {
            size_t seed = 0;
            hash_combine(seed, player.position);
            hash_combine(seed, player.score);
            return seed;
        }
    };

    template<> struct hash<State> {
        size_t operator()(const State& state) const {
            size_t seed = 0;
            hash_combine(seed, state.turn);
            hash_combine(seed, state.players[0]);
            hash_combine(seed, state.players[1]);
            return seed;
        }
    };
}

ostream& operator<<(ostream& out, const State& state) {
    out << "State {\n";
    out << "  turn: " << state.turn << ",\n"
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

State get_successor(const State& previous_state, int throw_result) {
    State state = previous_state;
    Player& current_player = state.players[state.turn % 2];
    current_player.position = (current_player.position-1 + throw_result)%10 + 1;
    current_player.score += current_player.position;
    ++state.turn;
    return state;
}

uint64_t k_throw_weight[] = {0, 0, 0, 1, 3, 6, 7, 6, 3, 1};

uint64_t quantum_sim(const State& initial_state) {
    uint64_t wins[] = {0, 0};
    queue<State> open;
    open.push(initial_state);
    unordered_set<State> closed;
    unordered_map<State,uint64_t> freq_count{{initial_state, 1}};
    while (!open.empty()) {
        State parent = open.front();
        open.pop();
        if (!closed.count(parent)) {
            closed.insert(parent);
            uint64_t freq_count_parent = freq_count[parent];
            if (parent.players[0].score >= 21) {
                wins[0] += freq_count_parent;
            }
            else if (parent.players[1].score >= 21) {
                wins[1] += freq_count_parent;
            }
            else {
                for (int throw_result = 3; throw_result <= 9; ++throw_result) {
                    State successor = get_successor(parent, throw_result);
                    open.push(successor);
                    freq_count[successor] += k_throw_weight[throw_result]*freq_count_parent;
                }
            }
        }
    }
    return max(wins[0], wins[1]);
}

int main() {
    State state;
    state.players[0].score = state.players[1].score = 0;
    state.turn = 0;
    string line1, line2;
    getline(cin, line1);
    getline(cin, line2);
    auto parts1 = split(line1);
    auto parts2 = split(line2);
    state.players[0].position = stoi(parts1.back());
    state.players[1].position = stoi(parts2.back());

    uint64_t ans = quantum_sim(state);
    cout << ans << endl;
}
