#include <iostream>
#include <queue>
#include <random>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
using namespace std;

enum class Direction { right, top, left, down };

typedef tuple<int,int> Point2i;

Point2i get_neighbor(const Point2i& point, Direction dir) {
    auto[x,y] = point;
    switch (dir) {
        case Direction::right:
            x += 1;
            break;
        case Direction::top:
            y -= 1;
            break;
        case Direction::left:
            x -= 1;
            break;
        case Direction::down:
            y += 1;
            break;
    }
    return Point2i(x,y);
}

const int k_wall = -2;
const int k_corridor = -1;

struct Cell {
    int room_number; // -2 means wall, -1 means corridor, >=0 is the room number
    bool can_halt;
};

const int k_nothing = -1;

struct Entity {
    int entity_class; // -1 means nothing, >=0 is one of {amber, bronze, copper, desert}
    int times_halted;

    bool operator==(const Entity& other) const {
        return entity_class == other.entity_class && times_halted == other.times_halted;
    }
};

struct Room {
    vector<Point2i> cells_within;
};

struct Environment {
    int width, height;
    vector<Cell> cells;
    vector<Room> rooms;

    Point2i get_coordinates(int cell_index) const {
        return Point2i(cell_index%width, cell_index/width);
    }

    int get_index(const Point2i& point) const {
        return get<1>(point)*width + get<0>(point);
    }

    const Cell& cell_at(const Point2i& point) const {
        return cells[get_index(point)];
    }

    Cell& cell_at(const Point2i& point) {
        return cells[get_index(point)];
    }
};

template<class T> inline void hash_combine(size_t& seed, const T& v) {
    hash<T> hasher;
    const size_t k_mul = 0x9ddfea08eb382d69ULL;
    size_t a = (hasher(v) ^ seed) * k_mul;
    a ^= (a >> 47);
    size_t b = (seed ^ a) * k_mul;
    b ^= (b >> 47);
    seed = b * k_mul;
}

struct Action {
    enum Verb { select, unselect, move };
    Verb verb;
    int argument;
};

ostream& operator<<(ostream& out, const Action& action) {
    const char* k_directions[] = {"right", "up", "left", "down"};
    switch (action.verb) {
        case Action::select:
            out << "select " << action.argument;
            break;
        case Action::unselect:
            out << "unselect";
            break;
        case Action::move:
            out << "move " << k_directions[action.argument];
            break;
    }
    return out;
}

int k_move_cost[] = {1, 10, 100, 1000};

struct State {
    const Environment* env;
    vector<Entity> entities;
    int active_entity;

    bool is_sorted() const {
        if (active_entity != -1)
            return false;
        for (int i = 0; i < entities.size(); ++i) {
            const Cell& cell = env->cells[i];
            const Entity& entity = entities[i];
            if (entity.entity_class != k_nothing && entity.entity_class != cell.room_number)
                return false;
        }
        return true;
    }

    State generate_successor(const Action& action) const {
        State succ = *this;
        switch (action.verb) {
            case Action::select:
                succ.active_entity = action.argument;
                break;
            case Action::unselect:
                ++succ.entities[succ.active_entity].times_halted;
                succ.active_entity = -1;
                break;
            case Action::move: {
                Direction dir = static_cast<Direction>(action.argument);
                Point2i src = env->get_coordinates(active_entity);
                Point2i dst = get_neighbor(src, dir);
                succ.entity_at(dst) = succ.entities[succ.active_entity];
                succ.entities[active_entity] = Entity{k_nothing, 0};
                succ.active_entity = env->get_index(dst);
                break;
            }
        }
        return succ;
    }

    int action_cost(const Action& action) const {
        if (action.verb == Action::move) {
            int entity_class = entities[active_entity].entity_class;
            return k_move_cost[entity_class];
        }
        return 0;
    }

    void fill_select_actions(vector<Action>& actions) const {
        for (int i = 0; i < entities.size(); ++i) {
            if (entities[i].entity_class != k_nothing && entities[i].times_halted < 2)
                actions.push_back(Action{Action::select, i});
        }
    }

    void fill_unselect_action(vector<Action>& actions) const {
        const Cell& cell = env->cells[active_entity];
        const Entity& entity = entities[active_entity];

        // cannot halt in cells in which the can_halt flag is false
        if (!cell.can_halt)
            return;

        // cannot halt in the corridor if it has halted already once
        if (entity.times_halted == 1 && cell.room_number == k_corridor)
            return;

        if (cell.room_number >= 0) {

            // cannot halt in a room that is not assigned to it
            if (cell.room_number != entity.entity_class)
                return;

            // cannot halt in a room with mixed classes
            for (const Point2i& room_cell : env->rooms[cell.room_number].cells_within) {
                const auto& other_entity = entity_at(room_cell);
                if (other_entity.entity_class != k_nothing &&
                    other_entity.entity_class != entity.entity_class)
                    return;
            }

            // Cannot halt in the middle of the room
            const Cell& south_cell = env->cells[active_entity+env->width];
            const Entity& south_entity = entities[active_entity+env->width];
            if (south_cell.room_number != k_wall && south_entity.entity_class == k_nothing)
                return;
        }

        actions.push_back(Action{Action::unselect, 0});
    }

    void fill_move_actions(vector<Action>& actions) const {
        const Cell& cell = env->cells[active_entity];
        const Entity& entity = entities[active_entity];
        Point2i src = env->get_coordinates(active_entity);
        for (int dir = 0; dir < 4; ++dir) {
            Point2i dst = get_neighbor(src, static_cast<Direction>(dir));
            const Cell& cell_dst = env->cell_at(dst);
            const Entity& entity_dst = entity_at(dst);
            // the destination cell must be empty
            if (cell_dst.room_number == k_wall || entity_dst.entity_class != k_nothing)
                continue;
            // cannot enter in a room that it's not its own
            if (cell.room_number == k_corridor && cell_dst.room_number >= 0 && cell_dst.room_number != entity.entity_class)
                continue;
            actions.push_back(Action{Action::move, dir});
        }
    }

    vector<Action> get_available_actions() const {
        vector<Action> actions;
        if (active_entity == -1)
            fill_select_actions(actions);
        else {
            fill_unselect_action(actions);
            fill_move_actions(actions);
        }
        return actions;
    }

    const Entity& entity_at(const Point2i& point) const {
        return entities[env->get_index(point)];
    }

    Entity& entity_at(const Point2i& point) {
        return entities[env->get_index(point)];
    }

    bool operator==(const State& other) const {
        return entities == other.entities && active_entity == other.active_entity;
    }
};

ostream& operator<<(ostream& out, const State& state) {
    for (int y = 0; y < state.env->height; ++y) {
        if (y > 0)
            out << '\n';
        for (int x = 0; x < state.env->width; ++x) {
            const Cell& cell = state.env->cell_at({x,y});
            const Entity& entity = state.entity_at({x,y});
            //if (cell.room_number == k_wall)
                //out << '#';
            //else if (cell.room_number == k_corridor && cell.can_halt)
                //out << ' ';
            //else if (cell.room_number == k_corridor && !cell.can_halt)
                //out << 'F';
            //else
                //out << cell.room_number;
            if (cell.room_number == k_wall)
                out << '#';
            else if (entity.entity_class == k_nothing)
                out << '.';
            else
                out << "ABCD"[entity.entity_class];
        }
    }
    return out;
}

namespace std {
    template <> struct hash<Entity> {
        size_t operator()(const Entity& entity) const {
            return ((entity.entity_class+1)<<2) | entity.times_halted;
        }
    };

    template <> struct hash<State> {
        size_t operator()(const State& state) const {
            size_t seed = hash<int>{}(state.active_entity);
            for (const Entity& entity : state.entities)
                hash_combine(seed, entity);
            return seed;
        }
    };
}

int min_sorting_cost(const State& initial_state) {
    typedef tuple<int,State> CostState;
    struct CostStateSort {
        bool operator()(const CostState& cs1, const CostState& cs2) const {
            return get<0>(cs1) > get<0>(cs2);
        }
    };
    priority_queue<CostState, vector<CostState>, CostStateSort> open;
    open.emplace(0, initial_state);

    unordered_map<State,int> best_cost_so_far;
    while (!open.empty()) {
        auto[cost,state] = move(open.top());
        open.pop();
        if (state.is_sorted()) {
            cout << best_cost_so_far.size() << endl;
            return cost;
        }
        auto actions = state.get_available_actions();
        for (const Action& action : actions) {
            //cout << action << endl;
            State successor = state.generate_successor(action);
            int cost_succ = cost + state.action_cost(action);
            auto it = best_cost_so_far.find(successor);
            if (it == best_cost_so_far.end()) {
                best_cost_so_far.emplace(successor, cost_succ);
                open.emplace(cost_succ, successor);
                //cout << successor << endl << endl;
            }
            else if (cost_succ < it->second) {
                it->second = cost_succ;
                open.emplace(cost_succ, successor);
                //cout << successor << endl << endl;
            }
        }
    }

    cout << best_cost_so_far.size() << endl;
    return -1;
}

State read_input(istream& in, Environment& env) {
    State state;
    state.env = &env;
    state.active_entity = -1;

    env.cells.clear();
    env.rooms = vector<Room>(4);

    string line;
    int line_no = 0;
    while (getline(in, line) && !line.empty()) {
        if (line_no == 0)
            env.width = line.size();
        int room_no = 0;
        for (int col = 0; col < env.width; ++col) {
            char c = col<line.size()? line[col] : ' ';

            Cell new_cell = {k_wall, false};
            Entity new_entity = {k_nothing, 0};

            if (c != '#' && c != ' ') {
                new_cell.can_halt = true;
                if (line_no == 1)
                    new_cell.room_number = k_corridor;
                else {
                    env.rooms[room_no].cells_within.emplace_back(col, line_no-1);
                    new_cell.room_number = room_no++;
                    if (line_no == 2)
                        env.cell_at({col,line_no-1}).can_halt = false;
                }
                if (c != '.')
                    new_entity.entity_class = c - 'A';
            }
            env.cells.push_back(new_cell);
            state.entities.push_back(new_entity);
        }
        ++line_no;
    }
    env.height = line_no;

    return state;
}


int main() {
    mt19937 rng(random_device{}());

    Environment env;
    State initial_state = read_input(cin, env);

    cout << initial_state << endl;
    cout << min_sorting_cost(initial_state) << endl;
    
    //while (!initial_state.is_sorted()) {
        //auto actions = initial_state.get_available_actions();
        //for (int i = 0; i < actions.size(); ++i) {
            //cout << i << ": " << actions[i] << endl;
        //}
        //int chosen;
        //cin >> chosen;
        //initial_state = initial_state.generate_successor(actions[chosen]);
        //cout << initial_state << endl;
    //}


    //for (int i = 0; i < 15; ++i) {
        //auto actions = initial_state.get_available_actions();
        //if (actions.empty()) {
            //cout << "no more actions available" << endl;
            //break;
        //}

        //uniform_int_distribution<> unif(0, actions.size()-1);
        //int selected = unif(rng);

        //initial_state = initial_state.generate_successor(actions[selected]);

        //cout << endl << actions[selected] << endl << initial_state << endl;
    //}

    //vector<State> states;

    //State initial_state;
    //cin >> initial_state;
    //cout << initial_state << endl;

    //auto successors = initial_state.generate_successors();
    //for (const auto&[_,state] : successors) {
        //auto grand = state.generate_successors();
        //for (const auto&[_,grandchild] : grand) {
            //states.push_back(grandchild);
        //}
    //}

    //for (const auto& state : states) {
        //cout << state << endl << endl;
    //}
}
