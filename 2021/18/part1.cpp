#include <cassert>
#include <iostream>
#include <string>
#include <utility>
using namespace std;

struct SnailNumber {
    SnailNumber* parent;
    union {
        SnailNumber* children[2];
        int value;
    };
    bool is_compound;
    
    
    SnailNumber* copy() const {
        SnailNumber* result = new SnailNumber;
        result->parent = nullptr;
        result->is_compound = is_compound;
        if (is_compound) {
            result->children[0] = children[0]->copy();
            result->children[1] = children[1]->copy();
            result->children[0]->parent = result;
            result->children[1]->parent = result;
        }
        else {
            result->value = value;
        }
        return result;
    }
    
    SnailNumber* sum(const SnailNumber* other) const {
        auto copy_this = copy();
        auto copy_other = other->copy();
        auto result = copy_this->join(copy_other);
        result->reduce();
        return result;
    }
    
    bool is_pair_of_regular_numbers() const {
        return is_compound && !children[0]->is_compound && !children[1]->is_compound;
    }
    
    void explode() {
        assert(is_pair_of_regular_numbers());
        auto left = get_side_leaf(0);
        auto right = get_side_leaf(1);
        if (left) {
            //~ cout << "left: " << *left << endl;
            left->value += children[0]->value;
        }
        if (right) {
            //~ cout << "right: " << *right << endl;
            right->value += children[1]->value;
        }
        is_compound = false;
        delete children[0];
        delete children[1];
        value = 0;
    }
    
    void split() {
        assert(!is_compound);
        int value_bk = value;
        is_compound = true;
        children[0] = new SnailNumber;
        children[0]->parent = this;
        children[0]->is_compound = false;
        children[0]->value = value_bk/2;
        children[1] = new SnailNumber;
        children[1]->parent = this;
        children[1]->is_compound = false;
        children[1]->value = value_bk/2 + value_bk%2;
    }
    
    bool reduce_explode(bool stop_after_first_explode = false, int depth = 0) {
        bool action_applied = false;
        if (depth >= 4 && is_pair_of_regular_numbers()) {
            //~ cout << "exploding " << *this << endl; 
            explode();
            action_applied = true;
        }
        else if (is_compound) {
            action_applied = children[0]->reduce_explode(stop_after_first_explode, depth+1);
            if (!action_applied || !stop_after_first_explode) {
                action_applied = children[1]->reduce_explode(stop_after_first_explode, depth+1) || action_applied;
            }
        }
        return action_applied;
    }
    
    bool reduce_split(bool stop_after_first_explode = false) {
        bool action_applied = false;
        if (!is_compound && value >= 10) {
            split();
            action_applied = true;
        }
        else if (is_compound) {
            action_applied = children[0]->reduce_split(stop_after_first_explode);
            if (!action_applied || !stop_after_first_explode) {
                action_applied = children[1]->reduce_split(stop_after_first_explode) || action_applied;
            }
        }
        return action_applied;
    }
    
    void reduce() {
        bool action_applied = false;
        do {
            action_applied = reduce_explode(false);
            action_applied = action_applied || reduce_split(true);
            //~ cout << *this << endl;
        } while (action_applied);
    }
    
    int get_magnitude() const {
        if (!is_compound) {
            return value;
        }
        int magnitude_left = children[0]->get_magnitude();
        int magnitude_right = children[1]->get_magnitude();
        return 3*magnitude_left + 2*magnitude_right;
    }
    
    SnailNumber* join(SnailNumber* other) {
        SnailNumber* result = new SnailNumber;
        result->parent = nullptr;
        result->is_compound = true;
        result->children[0] = this;
        result->children[1] = other;
        this->parent = result;
        other->parent = result;
        return result;
    }
    
    SnailNumber* get_deepest_leaf(int side) {
        auto current = this;
        while (current->is_compound) {
            current = current->children[side];
        }
        return current;
    }
    
    SnailNumber* get_side_leaf(int side) {
        auto cur = this;
        while (cur->parent != nullptr && cur->parent->children[side] == cur) {
            cur = cur->parent;
        }
        cur = cur->parent;
        if (cur != nullptr) {
            cur = cur->children[side]->get_deepest_leaf(1-side);
        }
        return cur;
    }
    
    ~SnailNumber() {
        if (is_compound) {
            delete children[0];
            delete children[1];
        }
    }
};

ostream& operator<<(ostream& out, const SnailNumber& number) {
    if (number.is_compound) {
        out << '[' << *number.children[0] << ',' << *number.children[1] << ']';
    }
    else {
        out << number.value;
    }
    return out;
}

SnailNumber* parse_next_snail_number(const string& strnum, int& index, SnailNumber* parent = nullptr) {
    auto number = new SnailNumber;
    number->parent = parent;
    if (strnum[index] == '[') {
        ++index;
        number->is_compound = true;
        number->children[0] = parse_next_snail_number(strnum, index, number);
        assert(strnum[index] == ',');
        ++index;
        number->children[1] = parse_next_snail_number(strnum, index, number);
        assert(strnum[index] == ']');
        ++index;
    }
    else {
        int end_pos = strnum.find_first_of(",]", index);
        number->is_compound = false;
        number->value = stoi(strnum.substr(index, end_pos-index));
        index = end_pos;
    }
    return number;
}

SnailNumber* parse_snail_number(const string& strnum) {
    int index = 0;
    auto number = parse_next_snail_number(strnum, index);
    assert(index == strnum.size());
    return number;
}

//~ void inorder(SnailNumber* num) {
    //~ auto left = num->get_side_leaf(0);
    //~ auto right = num->get_side_leaf(1);
    
    //~ cout << *num << ", left: ";
    //~ if (left) cout << *left;
    //~ else cout << "(none)";
    //~ cout << ", right: ";
    //~ if (right) cout << *right;
    //~ else cout << "(none)";
    //~ cout << ", parent: ";
    //~ if (num->parent) cout << *num->parent;
    //~ else cout << "(none)";
    //~ cout << endl;
    //~ if (num->is_compound) {
        //~ inorder(num->children[0]);
        //~ inorder(num->children[1]);
    //~ }
//~ }

int main() {
    string line;
    SnailNumber* running_sum = nullptr;
    while (getline(cin, line)) {
        auto num = parse_snail_number(line);
        if (running_sum) {
            auto sum = running_sum->sum(num);
            delete running_sum;
            delete num;
            running_sum = sum;
        }
        else {
            running_sum = num;
        }
    }
    cout << running_sum->get_magnitude() << endl;
}
