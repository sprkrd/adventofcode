#pragma once

#include <functional>
#include <string>
#include <vector>

namespace ash {

std::vector<int> every_int(const std::string& str);

template <typename T>
inline void hash_combine(uint64_t& seed, const T& val) {
    // https://github.com/HowardHinnant/hash_append/issues/7
    std::hash<T> hasher;
    seed ^= hasher(val) + 0x9e3779b97f4a7c15 + (seed<<12) + (seed>>4); 
}

} // namespace ash

