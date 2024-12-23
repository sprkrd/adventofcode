#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <optional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

std::vector<std::string> split(const std::string& str, const std::string& delim=" \n\t") {
    std::vector<std::string> parts;
    size_t pos = 0;
    while (pos < str.length())
    {
        size_t pos_next = str.find_first_of(delim, pos);
        if (pos_next > pos)
        {
            parts.push_back(str.substr(pos, pos_next-pos));
        }
        pos = pos_next == std::string::npos? std::string::npos : pos_next + 1;
    }
    return parts;
}

struct Trie
{
    Trie* child;
    Trie* sibling;
    char character;
    bool terminal;
    
    Trie(char character = '^') :
        child(nullptr),
        sibling(nullptr),
        character(character),
        terminal(false)
    {
    }
    
    Trie* findChild(char c) const
    {
        Trie* current = child;
        while (current && current->character != c)
        {
            current = current->sibling;
        }
        return current;
    }
    
    void insert(const std::string& word, size_t pos = 0)
    {
        if (pos == word.size())
        {
            terminal = true;
        }
        else
        {
            auto* foundChild = findChild(word[pos]);
            if (!foundChild)
            {
                foundChild = new Trie(word[pos]);
                foundChild->sibling = child;
                child = foundChild;
            }
            foundChild->insert(word, pos+1);
        }
    }
    
    ~Trie()
    {
        delete sibling;
        delete child;
    }
    
    void display(std::ostream& out, int identation = 0)
    {
        for (int i = 0; i < identation; ++i)
        {
            out << ' ';
        }
        out << character;
        if (terminal)
        {
            out << '*';
        }
        out << '\n';
        auto* currentChild = child;
        while (currentChild)
        {
            currentChild->display(out, identation+4);
            currentChild = currentChild->sibling;
        }
    }
};

struct TowelDP
{
    Trie towelRoot;
    mutable std::unordered_map<std::string,uint64_t> cache;
    
    void parseAvailableTowels(std::istream& in)
    {
        std::string line;
        std::getline(in, line);
        for (const auto& towel : split(line, ", "))
        {
            towelRoot.insert(towel);
        }
    }
    
    uint64_t call(const std::string& word) const
    {       
        if (word.empty())
        {
            return 1;
        }
        uint64_t result = 0;
        const Trie* trie = &towelRoot;
        for (size_t i = 0; i < word.size(); ++i)
        {
            trie = trie->findChild(word[i]);
            if (!trie)
            {
                break;
            }
            if (!trie->terminal)
            {
                continue;
            }
            result += (*this)(word.substr(i+1));
        }
        return result;
    }
    
    uint64_t operator()(const std::string& word) const
    {
        auto it = cache.find(word);
        if (it == cache.end())
        {
            uint64_t result = call(word);
            it = cache.emplace(word, result).first;
        }
        return it->second;
    }
};

int main()
{
    TowelDP towelDP;
    towelDP.parseAvailableTowels(std::cin);
    // towelDP.towelRoot.display(std::cout);
    
    uint64_t ans = 0;
    
    std::string word;
    while (std::cin >> word)
    {
        // std::cout << word << ": " << towelDP(word) << std::endl;
        ans += towelDP(word);
    }
    
    std::cout << ans << std::endl;
}
