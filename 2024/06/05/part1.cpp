#include <iostream>
#include <vector>
#include <string>
using namespace std;

typedef vector<int> Rule;
typedef vector<Rule> RuleBook;
typedef vector<int> Update;
typedef vector<Update> UpdateList;

vector<string> split(const string& str, const string& delim=" \n\t") {
    vector<string> parts;
    size_t pos = 0;
    while (pos < str.length())
    {
        size_t pos_next = str.find_first_of(delim, pos);
        if (pos_next > pos)
        {
            parts.push_back(str.substr(pos, pos_next-pos));
        }
        pos = pos_next == string::npos? string::npos : pos_next + 1;
    }
    return parts;
}

RuleBook parseRuleBook(istream& in)
{
    RuleBook ruleBook;
    string line;
    while (getline(cin, line) && !line.empty())
    {
        auto parts = split(line, "|");
        int lhs = stoi(parts[0]);
        int rhs = stoi(parts[1]);
        if (ruleBook.size() <= lhs)
        {
            ruleBook.resize(lhs+1);
        }
        ruleBook[lhs].push_back(rhs);
    }
    return ruleBook;
}

UpdateList parseUpdateList(istream& in)
{
    UpdateList updateList;
    string line;
    while (getline(cin, line) && !line.empty())
    {
        auto parts = split(line, ",");
        Update update(parts.size());
        for (unsigned i = 0; i < parts.size(); ++i)
        {
            update[i] = stoi(parts[i]);
        }
        updateList.push_back(move(update));
    }
    return updateList;
}

bool isValid(const RuleBook& ruleBook, const Update& update)
{
    vector<bool> seenEntry(ruleBook.size());
    for (int entry : update)
    {
        for (int preceded : ruleBook[entry])
        {
            if (seenEntry[preceded])
            {
                return false;
            }
        }
        seenEntry[entry] = true;
    }
    return true;
}

int main() {
    RuleBook ruleBook = parseRuleBook(cin);
    UpdateList updateList = parseUpdateList(cin);
    int ans = 0;
    for (const auto& update : updateList)
    {
        if (isValid(ruleBook, update))
        {
            ans += update[update.size()/2];
        }
    }
    cout << ans << endl;
}
