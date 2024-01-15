#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
using namespace std;

struct Entry {
    string data;
    vector<int> groups;

    void parse(const string& line) {
        istringstream iss(line);
        iss >> data;

        string numbers;
        iss >> numbers;
        groups.clear();
        size_t begin_token = 0;
        while (begin_token < numbers.size()) {
            size_t end_token = numbers.find_first_of(',', begin_token);
            if (end_token == string::npos) {
                groups.push_back(stoi(numbers.substr(begin_token)));
                begin_token = string::npos;
            }
            else {
                groups.push_back(stoi(numbers.substr(begin_token, end_token-begin_token)));
                begin_token = end_token + 1;
            }
        }
    }

    void unfold() {
        size_t  original_len = data.size();
        for (size_t i = 0; i < 4; ++i) {
            data.push_back('?');
            data.insert(data.end(), data.begin(), data.begin()+original_len);
        }

        original_len = groups.size();
        for (size_t i = 0; i < 4; ++i) {
            groups.insert(groups.end(), groups.begin(), groups.begin()+original_len);
        }
    }
};

ostream& operator<<(ostream& out, const Entry& entry) {
    out << entry.data << ' ';
    for (size_t i = 0; i < entry.groups.size(); ++i) {
        if (i) {
            out << ',';
        }
        out << entry.groups[i];
    }
    return out;
}

typedef pair<int,int> Arguments;

namespace std {

template<>
struct hash<Arguments> {
    size_t operator()(const Arguments& args) const noexcept {
        hash<int> h;
        return 10'000'019UL*h(args.first) + h(args.second);
    }
};

}

inline bool maybe_is(char c, char ref) {
    return c == ref || c == '?';
}

struct Arrangements {
    unordered_map<Arguments,size_t> cache;
    const Entry& entry;

    Arrangements(const Entry& entry) : entry(entry) {}

    size_t operator()(size_t i = 0, size_t j = 0) {
        auto[it,inserted] = cache.emplace(Arguments(i,j), 0);
        if (!inserted) {
            return it->second;
        }

        size_t ans = 0;
        if (i == entry.data.size()) {
            ans = j == entry.groups.size();
        }
        else {
            if (maybe_is(entry.data[i], '#') &&
                j<entry.groups.size() &&
                i+entry.groups[j]<=entry.data.size()) {
                size_t i_ = i+1;
                while (i_<i+entry.groups[j] && maybe_is(entry.data[i_],'#')) {
                    ++i_;
                }
                bool eof = i_ == entry.data.size();
                if (i_ == i + entry.groups[j] && (eof || maybe_is(entry.data[i_], '.'))) {
                    ans += (*this)(i_+!eof, j+1);
                }
            }
            if (maybe_is(entry.data[i], '.')) {
                ans += (*this)(i+1,j);
            }
        }
        it->second = ans;
        return ans;
    }


};

int main() {
    size_t sum = 0;
    string line;
    while (getline(cin, line)) {
        Entry entry;
        entry.parse(line);
        entry.unfold();
        Arrangements arr(entry);
        //cout << entry << ' ' << arr() << endl;
        sum += arr();
    }
    cout << sum << endl;
}
