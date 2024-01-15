#include <cassert>
#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;


uint8_t SEGMENT_A = 0x01;
uint8_t SEGMENT_B = 0x02;
uint8_t SEGMENT_C = 0x04;
uint8_t SEGMENT_D = 0x08;
uint8_t SEGMENT_E = 0x10;
uint8_t SEGMENT_F = 0x20;
uint8_t SEGMENT_G = 0x40;

uint8_t SEGMENTS[] = { SEGMENT_A, SEGMENT_B, SEGMENT_C, SEGMENT_D,
                       SEGMENT_E, SEGMENT_F, SEGMENT_G };
                       
map<uint8_t, int> decode(const vector<uint8_t>& distinct_digits) {
    // 2: 1
    // 3: 7
    // 4: 4
    // 5: 2, 3, 5
    // 6: 0, 6, 9
    // 7: 8
    map<uint8_t, int> codemap;
    uint8_t code_1 = 0, code_4 = 0;
    
    
    // the easy ones
    for (auto digit : distinct_digits) {
        int set_count = __builtin_popcount(digit);
        switch (set_count) {
            case 2:
                codemap[digit] = 1;
                code_1 = digit;
                break;
            case 3:
                codemap[digit] = 7;
                break;
            case 4:
                codemap[digit] = 4;
                code_4 = digit;
                break;
            case 7:
                codemap[digit] = 8;
                break;
        }
    }
    
    assert(codemap.size() == 4);
    
    // the hard ones
    for (auto digit : distinct_digits) {
        int set_count = __builtin_popcount(digit);
        if (set_count == 5) {
            if (__builtin_popcount(digit&code_1) == 2) {
                codemap[digit] = 3;
            }
            else if (__builtin_popcount(digit&code_4) == 2) {
                codemap[digit] = 2;
            }
            else {
                codemap[digit] = 5;
            }
        }
        else if (set_count == 6) {
            if (__builtin_popcount(digit&code_1) == 1) {
                codemap[digit] = 6;
            }
            else if (__builtin_popcount(digit&code_4) == 4) {
                codemap[digit] = 9;
            }
            else {
                codemap[digit] = 0;
            }
        }
    }

    return codemap;
}

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

uint8_t parse_segments(const string& segments_str) {
    uint8_t segments = 0;
    for (char s : segments_str) {
        segments |= SEGMENTS[s-'a'];
    }
    return segments;
}

string segments_to_string(uint8_t segments) {
    string segments_str;
    for (char s = 'a'; s <= 'g'; ++s) {
        if (segments & SEGMENTS[s-'a']) {
            segments_str += s;
        }
    }
    return segments_str;
}

struct Instance {
    vector<uint8_t> distinct_digits;
    vector<uint8_t> display_digits;
};

int main() {
    vector<Instance> instances;
    
    string line;
    while (getline(cin, line)) {
        auto parts = split(line, "|");
        auto parts_lhs = split(parts[0]);
        auto parts_rhs = split(parts[1]);
        instances.emplace_back();
        for (const auto& segments_str : parts_lhs) {
            instances.back().distinct_digits.push_back(
                parse_segments(segments_str));
        }
        for (const auto& segments_str : parts_rhs) {
            instances.back().display_digits.push_back(
                parse_segments(segments_str));
        }
    }
    
    int sum = 0;
    for (const auto& instance : instances) {
        auto codemap = decode(instance.distinct_digits);
        int num = 0;
        for (auto digit : instance.display_digits) {
            int digit_int = codemap.find(digit)->second;
            num = num*10 + digit_int;
        }
        sum += num;
    }
    cout << sum << endl;
    
    
}
