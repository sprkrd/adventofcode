#include <algorithm>
#include <iostream>
#include <list>
#include <queue>
#include <vector>

typedef std::list<int>::const_iterator Const_iterator;

inline int wrap(int x, int m) {
    return (x%m+m)%m;
}

Const_iterator find_destination(const std::list<int>& list, Const_iterator src) {
    auto dst = src;
    int displacement = wrap(*src, list.size()-1);
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

std::vector<int> mix(const std::vector<int>& input) {
    std::list<int> input_list(input.begin(), input.end());
    std::queue<Const_iterator> remaining_elements;
    for (auto it = input_list.begin(); it != input_list.end(); ++it) {
        remaining_elements.push(it);
    }
    while (!remaining_elements.empty()) {
        auto src = remaining_elements.front();
        remaining_elements.pop();
        auto dst = find_destination(input_list, src);
        input_list.splice(dst, input_list, src);
    }
    std::vector<int> ans(input_list.begin(), input_list.end());
    return ans;
}

std::ostream& operator<<(std::ostream& out, const std::vector<int>& v) {
    for (size_t i = 0; i < v.size(); ++i) {
        if (i > 0) out << ", ";
        out << v[i];
    }
    return out;
}

int main() {
    std::vector<int> numbers;
    int x;
    while (std::cin >> x) {
        numbers.push_back(x);
    }
    auto mixed = mix(numbers);
    //std::cout << mixed << std::endl;
    int index_0 = find(mixed.begin(), mixed.end(), 0) - mixed.begin();
    int element_1000 = wrap(index_0+1000, mixed.size());
    int element_2000 = wrap(index_0+2000, mixed.size());
    int element_3000 = wrap(index_0+3000, mixed.size());
    int ans = mixed[element_1000] + mixed[element_2000] + mixed[element_3000];
    std::cout << ans << std::endl;
}
