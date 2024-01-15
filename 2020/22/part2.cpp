#include <iostream>
#include <deque>
#include <set>
#include <string>
using namespace std;

struct state_t {
  deque<int> hand_p1, hand_p2;
  set<deque<int>> history_p1, history_p2;
};


ostream& operator<<(ostream& out, const deque<int>& deck) {
  bool first = true;
  for (int card : deck) {
    if (!first) out << ", ";
    out << card;
    first = false;
  }
  return out;
}

ostream& operator<<(ostream& out, const state_t& state) {
  return out << "Player 1's deck: " << state.hand_p1 << '\n'
             << "Player 2's deck: " << state.hand_p2;
}

void read_hand(istream& cin, deque<int>& hand) {
  string line;
  bool first_player_line = true;
  while (getline(cin,line) && !line.empty()) {
    if (!first_player_line)
      hand.push_back(stoi(line));
    first_player_line = false;
  }
}

int play_recursive_combat(state_t& state, int game_id = 1) {
  //cerr << "=== Game " << game_id << " ===\n";
  int round = 1;
  int next_game_id = game_id+1;
  while (!state.hand_p1.empty() && !state.hand_p2.empty()) {
    //cerr << "\n-- Round " << round << " (Game " << game_id << ") --\n"
         //<< state << '\n';
    if (state.history_p1.count(state.hand_p1) && state.history_p2.count(state.hand_p2)) {
      //cerr << "Repetition!\n";
      break;
    }
    state.history_p1.insert(state.hand_p1);
    state.history_p2.insert(state.hand_p2);
    int top1 = state.hand_p1.front();
    int top2 = state.hand_p2.front();
    //cerr << "Player 1 plays: " << top1 << '\n'
         //<< "Player 2 plays: " << top2 << '\n';
    state.hand_p1.pop_front();
    state.hand_p2.pop_front();
    int winner_round;
    if (top1 <= state.hand_p1.size() && top2 <= state.hand_p2.size()) {
      //cerr << "Playing a sub-game to determine the winner...\n\n";
      state_t sub_state = {{state.hand_p1.begin(),state.hand_p1.begin()+top1},
                           {state.hand_p2.begin(),state.hand_p2.begin()+top2},
                           {}, {}};
      winner_round = play_recursive_combat(sub_state, next_game_id++);
      //cerr << "\n...anyway, back to game " << game_id << '\n';
    }
    else
      winner_round = top1 > top2? 1 : 2;
    //cerr << "Player " << winner_round << " wins round " << round << " of game " << game_id << "!\n";
    if (winner_round == 1) {
      state.hand_p1.push_back(top1);
      state.hand_p1.push_back(top2);
    }
    else {
      state.hand_p2.push_back(top2);
      state.hand_p2.push_back(top1);
    }
    ++round;
  }
  int winner = state.hand_p1.empty()? 2 : 1;
  //cerr << "The winner of game " << game_id << " is player " << winner << "!\n";
  return winner;
}

int main() {
  state_t state;
  read_hand(cin,state.hand_p1);
  read_hand(cin,state.hand_p2);

  int winner = play_recursive_combat(state);
  deque<int>* winning_deck = winner == 1? &state.hand_p1 : &state.hand_p2;
  int score = 0;
  for (int i = 0; i < winning_deck->size(); ++i)
    score += (winning_deck->size()-i)*winning_deck->at(i);

  cout << "\n\n== Post-game results ==\n"
       << state << '\n';
  cout << "Score winner: " << score << endl;
}

