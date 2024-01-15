#include "utils.hpp"

#include <cassert>
#include <regex>


std::vector<int> ash::every_int(const std::string& str) {
    static const std::regex rex("-?[0-9]+");
    std::vector<int> ans;
    std::sregex_iterator it(str.begin(), str.end(), rex);
    for (; it != std::sregex_iterator(); ++it) {
        ans.push_back(stoi(it->str()));
    }
    return ans;
}

