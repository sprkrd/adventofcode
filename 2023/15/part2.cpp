#include <iostream>
#include <string>
#include <vector>
using namespace std;

size_t HASH(const string& str) {
    size_t seed = 0;
    for (char c : str) {
        seed = (seed + c)*17;
    }
    seed = seed%256;
    return seed;
}

typedef pair<string,int> Lens;
typedef vector<Lens> Box;

struct Environment {
    vector<Box> boxes;

    Environment() : boxes(256) {
    }

    void step(const string& action) {
        if (action.back() == '-') {
            string label = action.substr(0, action.size()-1);
            size_t h = HASH(label);
            for (auto it = boxes[h].begin(); it != boxes[h].end(); ++it) {
                if (it->first == label) {
                    boxes[h].erase(it);
                    break;
                }
            }
        }
        else {
            string label = action.substr(0, action.size()-2);
            int focal_length = action.back() - '0';
            size_t h = HASH(label);
            for (auto it = boxes[h].begin(); it != boxes[h].end(); ++it) {
                if (it->first == label) {
                    it->second = focal_length;
                    return;
                }
            }
            boxes[h].emplace_back(move(label), focal_length);
        }
    }

    size_t focusing_power() const {
        size_t ans = 0;
        for (size_t i = 0; i < boxes.size(); ++i) {
            for (size_t j = 0; j < boxes[i].size(); ++j) {
                ans += (i+1)*(j+1)*boxes[i][j].second;
            }
        }
        return ans;
    }
};

int main() {
    Environment env;
    string line;
    while (getline(cin,line,',')) {
        if (line.back()=='\n') {
            line.pop_back();
        }
        env.step(line);
    }
    cout << env.focusing_power() << endl;
}
