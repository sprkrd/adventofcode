#include <algorithm>
#include <iostream>
#include <queue>
#include <list>
#include <vector>

typedef int64_t integer;
typedef std::list<integer>::const_iterator Const_iterator;

constexpr integer decryption_key = 811589153;

inline integer wrap(integer x, integer m) {
    return (x%m+m)%m;
}

Const_iterator find_destination(const std::list<integer>& list, Const_iterator src) {
    auto dst = src;
    integer displacement = wrap(*src, list.size()-1);
    if (displacement > int(list.size()-1)/2) {
        displacement -= list.size()-1;
    }
    if (displacement > 0) {
        ++dst;
    }
    while (displacement > 0) {
        if (dst == list.end()) {
            dst = list.begin();
        }
        ++dst;
        --displacement;
    }
    while (displacement < 0) {
        if (dst == list.begin()) {
            dst = list.end();
        }
        --dst;
        ++displacement;
    }
    return dst;
}

std::vector<integer> mix(const std::vector<integer>& input, int times = 1) {
    std::list<integer> input_list(input.begin(), input.end());
    std::vector<Const_iterator> elements_in_order;
    elements_in_order.reserve(input_list.size());
    for (auto it = input_list.begin(); it != input_list.end(); ++it) {
        elements_in_order.push_back(it);
    }
    for (int i = 0; i < times; ++i) {
        for (auto src : elements_in_order) {
            auto dst = find_destination(input_list, src);
            input_list.splice(dst, input_list, src);
        }
    }
    std::vector<integer> ans(input_list.begin(), input_list.end());
    return ans;
}

std::ostream& operator<<(std::ostream& out, const std::vector<integer>& v) {
    for (size_t i = 0; i < v.size(); ++i) {
        if (i > 0) out << ", ";
        out << v[i];
    }
    return out;
}

int main() {
    std::vector<integer> numbers;
    integer x;
    while (std::cin >> x) {
        numbers.push_back(x*decryption_key);
    }
    //std::cout << numbers << std::endl;
    auto mixed = mix(numbers, 10);
    //std::cout << mixed << std::endl;
    int index_0 = find(mixed.begin(), mixed.end(), 0) - mixed.begin();
    int element_1000 = wrap(index_0+1000, mixed.size());
    int element_2000 = wrap(index_0+2000, mixed.size());
    int element_3000 = wrap(index_0+3000, mixed.size());
    integer ans = mixed[element_1000] + mixed[element_2000] + mixed[element_3000];
    std::cout << ans << std::endl;
}
