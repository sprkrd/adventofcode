#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

struct recipe_t {
  set<string> ingredients, allergens;
};

void print_set(ostream& out, const set<string>& s, const char* delim = ", ") {
  bool first = true;
  for (const auto& element : s) {
    if (!first) out << delim;
    out << element;
    first = false;
  }
}

ostream& operator<<(ostream& out, const recipe_t& recipe) {
  print_set(out, recipe.ingredients, " ");
  if (!recipe.allergens.empty()) {
    out << " (contains ";
    print_set(out, recipe.allergens);
    out << ')';
  }
  return out;
}

vector<recipe_t> parse_input(istream& in) {
  vector<recipe_t> recipes;
  string line;
  while (getline(cin,line)) {
    istringstream line_sstream(line);
    recipes.emplace_back();
    string word;
    while (line_sstream >> word && word != "(contains")
      recipes.back().ingredients.insert(word);
    while (line_sstream >> word) {
      if (word.back() == ',' || word.back() == ')')
        recipes.back().allergens.emplace(word.begin(), --word.end());
      else
        recipes.back().allergens.insert(word);
    }
  }
  return recipes;
}

void inplace_intersection(set<string>& s, const set<string>& other) {
  auto it_s = s.begin();
  auto it_other = other.begin();
  while (it_s != s.end() && it_other != other.end()) {
    if (*it_s < *it_other)
      it_s = s.erase(it_s);
    else if (*(it_other++) == *it_s)
      ++it_s;
  }
  s.erase(it_s, s.end());
}

void inplace_difference(set<string>& s, const set<string>& other) {
  auto it_s = s.begin();
  auto it_other = other.begin();
  while (it_s != s.end() && it_other != other.end()) {
    if (*it_s < *it_other)
      ++it_s;
    else if (*(it_other++) == *it_s)
      it_s = s.erase(it_s);
  }
}

int main() {
  auto recipes = parse_input(cin);
  for (const auto& recipe : recipes)
    cout << recipe << '\n';
  cout << '\n';
  map<string,set<string>> ingredients_per_allergen;
  for (const auto& recipe : recipes) {
    for (const auto& allergen : recipe.allergens) {
      auto it = ingredients_per_allergen.find(allergen);
      if (it == ingredients_per_allergen.end())
        ingredients_per_allergen.emplace(allergen, recipe.ingredients);
      else
        inplace_intersection(it->second, recipe.ingredients);
    }
  }
  for (const auto&[allergen,ingredients] : ingredients_per_allergen) {
    print_set(cout << allergen << ": ", ingredients);
    cout << '\n';
  }
  cout << '\n';

  map<string,string> assignments;
  while (assignments.size() != ingredients_per_allergen.size()) {
    auto it = find_if(ingredients_per_allergen.begin(), ingredients_per_allergen.end(),
        [](const auto& entry) { return entry.second.size() == 1; });
    assert(it != ingredients_per_allergen.end());
    auto ingredient = *(it->second.begin());
    assignments[it->first] = ingredient;
    for (auto&[allergen,ingredients] : ingredients_per_allergen)
      ingredients.erase(ingredient);
  }
  bool first = true;
  for (const auto&[allergen,ingredient] : assignments) {
    if (!first) cout << ',';
    cout << ingredient;
    first = false;
  }
  cout << endl;
}

