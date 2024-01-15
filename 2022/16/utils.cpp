#include "utils.hpp"

#include <cassert>
#include <regex>


int ash::find_rate(const std::string& str) {
    static const std::regex rex("[0-9]+");
    std::smatch m;
    assert(regex_search(str.begin(), str.end(), m, rex));
    return stoi(m.str());
}

std::vector<std::string> ash::every_valve(const std::string& str) {
    static const std::regex rex("[A-Z]{2,}");
    std::vector<std::string> ans;
    std::sregex_iterator it(str.begin(), str.end(), rex);
    for (; it != std::sregex_iterator(); ++it) {
        ans.push_back(it->str());
    }
    return ans;
}

