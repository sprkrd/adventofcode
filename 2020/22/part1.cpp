#include <iostream>
#include <deque>
#include <string>
using namespace std;

struct state_t {
  deque<int> hand_p1, hand_p2;
};

ostream& operator<<(ostream& out, const state_t& state) {
  out << "Player 1:";
  for (int card : state.hand_p1)
    out << ' ' << card;
  out << "\nPlayer 2:";
  for (int card : state.hand_p2)
    out << ' ' << card;
  return out;
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

void update(state_t& state) {
  deque<int>* winning_deck = &state.hand_p1;
  deque<int>* losing_deck = &state.hand_p2;
  if (state.hand_p1.front() < state.hand_p2.front()) {
    winning_deck = &state.hand_p2;
    losing_deck = &state.hand_p1;
  }
  winning_deck->push_back(winning_deck->front());
  winning_deck->push_back(losing_deck->front());
  winning_deck->pop_front();
  losing_deck->pop_front();
}

int main() {
  state_t state;
  read_hand(cin,state.hand_p1);
  read_hand(cin,state.hand_p2);

  cout << state << '\n';
  while (!state.hand_p1.empty() && !state.hand_p2.empty()) {
    update(state);
    cout << "\n-------\n" << state << '\n';
  }

  deque<int>* winner = state.hand_p1.empty()? 
    &state.hand_p2 : &state.hand_p1;

  int score = 0;
  for (int i = 0; i < winner->size(); ++i)
    score += (winner->size()-i)*winner->at(i);
  cout << "Score winner: " << score << endl;
}

