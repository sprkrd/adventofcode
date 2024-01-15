#include <iostream>
#include <map>
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

bool is_valid_passport(const passport_t& passport) {
  return passport.count("byr") &&
         passport.count("iyr") &&
         passport.count("eyr") &&
         passport.count("hgt") &&
         passport.count("hcl") &&
         passport.count("ecl") &&
         passport.count("pid");
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

