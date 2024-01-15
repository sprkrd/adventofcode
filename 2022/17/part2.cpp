#include <array>
#include <deque>
#include <iostream>
#include <map>
#include <random>
using namespace std;

typedef int64_t integer;

constexpr int k_playfield_height = 50;
constexpr int k_playfield_width = 7;
constexpr uint64_t k_empty_row = (1ULL<<(k_playfield_width+1))|1;
constexpr uint64_t k_floor = (1ULL<<(k_playfield_width+2)) - 1;

struct Sprite {
    uint64_t data;
    int height;
    int width;

    Sprite(int width, int height, uint64_t data = 0) : data(data), height(height), width(width) {
    }

    uint64_t row(int y) const {
        return (data>>(y*width))&((1ULL<<width)-1);
    }
};

string bits_to_string(uint64_t row) {
    string result;
    while (row) {
        result += (row&1? '*' : ' ');
        row >>= 1;
    }
    return result;
}

ostream& operator<<(ostream& out, const Sprite& sprite) {
    for (int y = sprite.height-1; y >= 0; --y) {
        out << bits_to_string(sprite.row(y)) << '\n';
    }
    return out;
}

const array<const Sprite, 5> sprites {
    Sprite(4, 1, 0b1111),
    Sprite(3, 3, 0b010111010),
    Sprite(3, 3, 0b100100111),
    Sprite(1, 4, 0b1111),
    Sprite(2, 2, 0b1111)
};


typedef array<uint64_t,k_playfield_height*128> Z_table;

Z_table zobrist_table() {
    mt19937_64 rng(42);
    Z_table table;
    for (auto& value : table) {
        value = rng();
    }
    return table;
}

struct Playfield {
    deque<uint64_t> rows;
    integer y_offset;
    integer top;

    Playfield() : rows(k_playfield_height, k_empty_row), y_offset(0), top(0) {
        rows[0] = k_floor;
    }

    uint64_t row(int y) {
        if (y-y_offset >= k_playfield_height) {
            return k_empty_row;
        }
        return rows[y-y_offset];
    }

    void add_row() {
        rows.push_back(k_empty_row);
    }

    void pan_up() {
        ++y_offset;
        rows.pop_front();
        rows.push_back(k_empty_row);
    }

    bool collides(const Sprite& sprite, integer x, integer y) {
        for (int y_sprite = 0; y_sprite < sprite.height; ++y_sprite) {
            uint64_t sprite_row = sprite.row(y_sprite) << (x+1);
            uint64_t playfield_row = row(y+y_sprite);
            if (sprite_row&playfield_row) {
                return true;
            }
        }
        return false;
    }

    void draw(const Sprite& sprite, integer x, integer y) {
        while (y_offset+k_playfield_height < y+sprite.height) {
            pan_up();
        }
        for (int y_sprite = 0; y_sprite < sprite.height; ++y_sprite) {
            uint64_t sprite_row = sprite.row(y_sprite) << (x+1);
            rows[y-y_offset+y_sprite] |= sprite_row;
        }
        top = max(top, y+sprite.height-1);
    }

    uint64_t z_hash() const {
        static Z_table z_table = zobrist_table();
        uint64_t result = 0;
        for (int i = 0; i < k_playfield_height; ++i) {
            uint64_t r = (rows[i]>>1)&127;
            result ^= z_table[i*128+r];
        }
        return result;
    }
};

ostream& operator<<(ostream& out, const Playfield& playfield) {
    for (auto it = playfield.rows.rbegin(); it != playfield.rows.rend(); ++it) {
        out << bits_to_string(*it) << '\n';
    }
    return out;
}

struct System {
    Playfield playfield;
    const string& input;
    int sprite_index = 0;
    int jet_index = 0;
    integer rocks = 0;
    integer falling_x = -1;
    integer falling_y = -1;

    System(const string& input) : input(input) {}

    bool ministep() {
        if (falling_x == -1) {
            falling_x = 2;
            falling_y = playfield.top + 4;
            return false;
        }
        const Sprite& sprite = sprites[sprite_index];
        integer tentative_x = input[jet_index]=='>'?
            (falling_x+1) :
            (falling_x-1);
        jet_index = (jet_index+1) % input.length();
        if (!playfield.collides(sprite, tentative_x, falling_y)) {
            falling_x = tentative_x;
        }
        if (playfield.collides(sprite, falling_x, falling_y-1)) {
            playfield.draw(sprite, falling_x, falling_y);
            sprite_index = (sprite_index+1)%sprites.size();
            ++rocks;
            falling_x = falling_y = -1;
            return true;
        }
        --falling_y;
        return false;
    }

    integer tower_height() const {
        return playfield.top;
    }

    void step() {
        while (!ministep());
    }

    array<integer,4> find_period() {
        typedef tuple<int,int,uint64_t> State;
        typedef pair<integer,integer> Rocks_and_height;
        array<integer,4> answer;
        map<State,Rocks_and_height> checkpoint;
        auto& mu = answer[0];
        auto& lam = answer[1];
        auto& height_mu = answer[2];
        auto& height_lam = answer[3];
        while (1) {
            step();
            if (tower_height() >= 8) {
                State state(sprite_index, jet_index, playfield.z_hash());
                auto[it,inserted] = checkpoint.emplace(state, Rocks_and_height(rocks, playfield.top));
                if (!inserted) {
                    mu = (it->second).first;
                    lam = rocks - (it->second).first;
                    height_mu = (it->second).second;
                    height_lam = playfield.top;
                    break;
                }
            }
        }
        return answer;
    }
};

int main() {
    const integer target = 1000000000000LL;

    string input;
    getline(cin, input);

    System system(input);

    auto[mu,lam,height_mu,height_lam] = system.find_period();


    integer loop_increase = height_lam - height_mu;
    integer num_loops = (target - mu - lam) / lam;
    integer rocks_remaining = (target - mu - lam) % lam;

    for (integer i = 0; i < rocks_remaining; ++i) {
        system.step();
    }

    integer tower_height = num_loops*loop_increase + system.tower_height();

    cout << tower_height << endl;
    
    //for (int i = 0; i < 2022; ++i) {
        //cout << system.playfield.z_hash() << endl;
        //system.step();
    //}
    //cout << system.playfield << endl;
    //cout << system.playfield.z_hash() << endl;
    //cout << system.tower_height() << endl;
}
