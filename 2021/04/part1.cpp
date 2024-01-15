#include <array>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

//   0   1   2   3   4
//   5   6   7   8   9
//  10  11  12  13  14
//  15  16  17  18  19
//  20  21  22  23  24

int LINES[][5] = {
    {0, 1, 2, 3, 4},
    {5, 6, 7, 8, 9},
    {10, 11, 12, 13, 14},
    {15, 16, 17, 18, 19},
    {20, 21, 22, 23, 24},
    {0, 5, 10, 15, 20},
    {1, 6, 11, 16, 21},
    {2, 7, 12, 17, 22},
    {3, 8, 13, 18, 23},
    {4, 9, 14, 19, 24}
};

typedef array<int, 25> Board;

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

bool check(const Board& board) {
    for (const auto& line : LINES) {
        bool wins = true;
        for (int i : line) {
            if (board[i] != -1) {
                wins = false;
                break;
            }
        }
        if (wins) {
            return true;
        }
    }
    return false;
}

bool mark_and_check(Board& board, int number) {
    for (int& num : board) {
        if (num == number) {
            num = -1;
            return check(board);
        }
    }
    return false;
}

int sum_unmarked(const Board& board) {
    int sum = 0;
    for (int num : board) {
        if (num != -1) {
            sum += num;
        }
    }
    return sum;
}

int main() { 
  string line;
  cin >> line;
  vector<int> numbers;
  for (const string& num : split(line, ",")) {
      numbers.push_back(stoi(num));
  }

  vector<Board> boards;
  int first;
  while (cin >> first) {
      boards.emplace_back();
      boards.back()[0] = first;
      for (int i = 1; i < 25; ++i) {
          cin >> boards.back()[i];
      }
  }

  for (int num : numbers) {
      for (Board& board : boards) {
          if (mark_and_check(board, num)) {
              int ans = sum_unmarked(board)*num;
              cout << ans << endl;
              return 0;
          }
      }
  }
  cout << "This should not happen" << endl;
  return -1;

}
