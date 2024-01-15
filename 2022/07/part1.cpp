#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <vector>
using namespace std;

constexpr int k_max_size = 100000;

struct Node {
    Node* parent;
    unordered_map<string,Node*> children;
    mutable int cached_size;
    bool is_dir;

    Node(Node* parent = nullptr) : parent(parent), cached_size(-1), is_dir(true) {}

    Node(int size, Node* parent = nullptr) : parent(parent), cached_size(size), is_dir(false) {}

    int size() const {
        if (cached_size >= 0)
            return cached_size;
        cached_size = 0;
        for (const auto&[_,child] : children) {
            cached_size += child->size();
        }
        return cached_size;
    }

    Node* root() {
        Node* current = this;
        while (current->parent != nullptr)
            current = current->parent;
        return current;
    }

    Node* cd(const string& name) {
        assert(is_dir);
        if (name == "..")
            return parent;
        else if (name == "/")
            return root();
        auto it = children.find(name);
        if (it == children.end())
            tie(it,ignore) = children.insert({name, new Node(this)});
        return it->second;
    }

    Node* touch(const string& name, int size) {
        assert(is_dir);
        auto it = children.find(name);
        if (it == children.end())
            tie(it,ignore) = children.insert({name, new Node(size, this)});
        return it->second;
    }

    template <class Predicate>
    void filter(vector<const Node*>& nodes, const Predicate& predicate) const {
        if (predicate(this))
            nodes.push_back(this);
        for (const auto& [_,child] : children) {
            child->filter(nodes, predicate);
        }
    }

    template <class Predicate>
    vector<const Node*> filter(const Predicate& predicate) const {
        vector<const Node*> filtered_nodes;
        filter(filtered_nodes, predicate);
        return filtered_nodes;
    }

    void print(ostream& out, const string& name = "/", int indent = 0) const {
        for (int i = 0; i < indent; ++i) {
            out << ' ';
        }
        out << "- " << name << " (";
        if (is_dir)
            out << "dir";
        else
            out << "file, size=" << cached_size;
        out << ")\n";
        for (const auto&[name,child] : children) {
            child->print(out, name, indent+2);
        }
    }
};

ostream& operator<<(ostream& out, const Node& filesystem) {
    filesystem.print(out);
    return out;
}

int main() {
    Node* current = new Node;
    string line;
    getline(cin, line);
    while (cin) {
        istringstream iss(line);
        char prompt;
        string cmd;
        iss >> prompt >> cmd;
        assert(prompt == '$');
        if (cmd == "cd") {
            string arg;
            iss >> arg;
            current = current->cd(arg);
            getline(cin, line);
        }
        else {
            while (getline(cin, line) && line[0] != '$') {
                iss = istringstream(line);
                string property, name;
                iss >> property >> name;
                if (property == "dir") {
                    current->cd(name);
                }
                else {
                    current->touch(name, stoi(property));
                }
            }
        }
    }
    //cout << *(current->root()) << endl;
    auto nodes = current->root()->filter([](const Node* node) { return node->is_dir && node->size() < k_max_size; });
    int ans = 0;
    for (const Node* node : nodes)
        ans += node->size();
    cout << ans << endl;
}
