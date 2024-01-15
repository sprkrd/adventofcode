#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>
using namespace std;

typedef map<string,string> passport_t;

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

istream& read_passport(istream& in, passport_t& passport) {
  string passport_str;
  string line;
  while (getline(in,line) && !line.empty())
    passport_str += line + "\n";
  passport.clear();
  auto parts = split(passport_str);
  for (const auto& p : parts) {
    auto key_and_value = split(p,":");
    passport.insert( {move(key_and_value[0]), move(key_and_value[1])} );
  }
  return in;
}

bool is_valid_field(const passport_t& passport, const string& field, regex re) {
  auto it = passport.find(field);
  if (it == passport.end())
    return false;
  return regex_match(it->second, re);
}

bool is_valid_byr(const passport_t& passport) {
  return is_valid_field(passport, "byr", regex("19[2-9]\\d|200[012]"));
}

bool is_valid_iyr(const passport_t& passport) {
  return is_valid_field(passport, "iyr", regex("201\\d|2020"));
}

bool is_valid_eyr(const passport_t& passport) {
  return is_valid_field(passport, "eyr", regex("202\\d|2030"));
}

bool is_valid_hgt(const passport_t& passport) {
  return is_valid_field(passport, "hgt", regex("(?:1[5-8]\\d|19[0-3])cm|(?:59|6\\d|7[0-6])in"));
}

bool is_valid_hcl(const passport_t& passport) {
  return is_valid_field(passport, "hcl", regex("#[\\da-f]{6}"));
}

bool is_valid_ecl(const passport_t& passport) {
  return is_valid_field(passport, "ecl", regex("amb|blu|brn|gry|grn|hzl|oth"));
}

bool is_valid_pid(const passport_t& passport) {
  return is_valid_field(passport, "pid", regex("\\d{9}"));
}

bool is_valid_passport(const passport_t& passport) {
  return is_valid_byr(passport) && is_valid_iyr(passport) &&
         is_valid_eyr(passport) && is_valid_hgt(passport) &&
         is_valid_hcl(passport) && is_valid_ecl(passport) &&
         is_valid_pid(passport);
}

int main() {
  passport_t passport;
  int number_of_valid_passports = 0;
  read_passport(cin, passport);
  while (!passport.empty()) {
    number_of_valid_passports += is_valid_passport(passport);
    read_passport(cin, passport);
  }
  cout << number_of_valid_passports << endl;
}

