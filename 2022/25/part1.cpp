#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

constexpr int k_midpoint = 2;
constexpr int k_base = 2*k_midpoint + 1;
const char* digits_representation = "=-012";

int snafu_digit_to_int(char c) {
    switch (c) {
        case '=':
            return -2;
        case '-':
            return -1;
        default:
            return c - '0';
    }
}

pair<int,int> half_adder(int a, int b) {
    assert(abs(a) <= k_midpoint && abs(b) <= k_midpoint);
    int sum = a + b;
    int carry = (sum>k_midpoint) - (sum<-k_midpoint);
    sum -= carry * k_base;
    return {sum, carry};
}

pair<int,int> full_adder(int a, int b, int c = 0) {
    assert(abs(a) <= k_midpoint && abs(b) <= k_midpoint && abs(c) <= k_midpoint);
    auto[partial_sum, carry_1] = half_adder(a, b);
    auto[sum, carry_2] = half_adder(partial_sum, c);
    int carry = carry_1 + carry_2;
    return {sum, carry};
}

vector<int> snafu_digits_little_endian(const string& snafu) {
    vector<int> result(snafu.length());
    for (size_t i = 0; i < snafu.length(); ++i) {
        result[snafu.length() - i - 1] = snafu_digit_to_int(snafu[i]);
    }
    return result;
}

string digits_little_endian_to_snafu(const vector<int>& digits) {
    if (digits.empty()) {
        return "0";
    }
    string snafu(digits.size(), '0');
    for (size_t i = 0; i < digits.size(); ++i) {
        snafu[digits.size() - i - 1] = digits_representation[digits[i]+k_midpoint];
    }
    return snafu;
}

string snafu_sum(const string& l, const string& r) {
    auto digits_le_l = snafu_digits_little_endian(l);
    auto digits_le_r = snafu_digits_little_endian(r);
    size_t size = max(digits_le_l.size(), digits_le_r.size());
    digits_le_l.resize(size, 0);
    digits_le_r.resize(size, 0);
    vector<int> sum;
    int carry = 0;
    for (size_t i = 0; i < size; ++i) {
        auto[digit, next_carry] = full_adder(digits_le_l[i], digits_le_r[i], carry);
        sum.push_back(digit);
        carry = next_carry;
    }
    if (carry != 0) {
        sum.push_back(carry);
    }
    while (sum.back() == 0) {
        sum.pop_back();
    }
    return digits_little_endian_to_snafu(sum);
}

int64_t snafu_to_int(const string& snafu) {
    int64_t result = 0;
    for (char c : snafu) {
        result = result*k_base + snafu_digit_to_int(c);
    }
    return result;
}

string int_to_snafu(int64_t number) {
    if (number == 0) {
        return "0";
    }
    string result;
    while (number) {
        auto r = (number + k_midpoint) % k_base - k_midpoint;
        number = (number + k_midpoint) / k_base;
        result.push_back(digits_representation[r+k_midpoint]);
    }
    reverse(result.begin(), result.end());
    return result;
}

int main() {
    vector<string> snafu_list;
    string line;
    while (getline(cin, line) && !line.empty()) {
        snafu_list.push_back(move(line));
    }
    // option 1: directly sum in snafu format
    {
        string result = "0";
        for (const auto& snafu : snafu_list) {
            result = snafu_sum(snafu, result);
        }
        cout << result << endl;
    }
    // option 2: convert to int, sum in int, convert back to snafu
    {
        uint64_t result = 0;
        for (const auto& snafu : snafu_list) {
            result += snafu_to_int(snafu);
        }
        cout << int_to_snafu(result) << endl;
    }
}
