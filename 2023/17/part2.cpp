#include <algorithm>
#include <cassert>
#include <climits>
#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <tuple>
#include <unordered_map>
using namespace std;


enum class Direction { north, west, south, east};

constexpr Direction all_directions[] = {
    Direction::north,
    Direction::west,
    Direction::south,
    Direction::east
};

typedef pair<int,int> Vec2i;

typedef pair<Vec2i,Direction> State;

Vec2i operator+(const Vec2i& l, const Vec2i& r) {
    return Vec2i(l.first+r.first, l.second+r.second);
}

Vec2i operator*(int s, const Vec2i& v) {
    return Vec2i(s*v.first, s*v.second);
}

ostream& operator<<(ostream& out, const Vec2i& v) {
    return out << '(' << v.first << ", " << v.second << ')';
}

ostream& operator<<(ostream& out, Direction dir) {
    static constexpr const char* names[] = {"north", "west", "south", "east"};
    return out << names[static_cast<int>(dir)];
}

Vec2i dir2vec(Direction dir) {
    static constexpr Vec2i vectors[] = {{0,-1}, {-1,0}, {0,1}, {1,0}};
    return vectors[static_cast<int>(dir)];
}

Direction opposite(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir)+2)%4);
}

struct StateHasher {
    size_t operator()(const State& state) const noexcept {
        hash<int> int_hasher;
        size_t seed = int_hasher(state.first.first);
        seed = seed*10'000'009 + int_hasher(state.first.second);
        seed = seed*10'000'009 + int_hasher(static_cast<int>(state.second));
        return seed;
    }
};

struct Action {
    Direction move_dir;
    int stride;
};

struct Map {
    int nrows;
    int ncols;
    string data;

    Map() {}

    Map(int nrows, int ncols, char def = '.') :
        nrows(nrows), ncols(ncols), data(nrows*ncols, def) {
    }

    int count(char c) const {
        return std::count(data.begin(), data.end(), c);
    }
    
    void read(istream& in) {
        nrows = 0;
        data.clear();
        string row;
        while (getline(in,row)) {
            ++nrows;
            ncols = row.size();
            data.insert(data.end(), row.begin(), row.end());        }
    }

    int ravel(const Vec2i& position) const {
        return position.second*ncols + position.first;
    }

    char at(const Vec2i& position) const {
        return data[ravel(position)];
    }

    char& at(const Vec2i& position) {
        return data[ravel(position)];
    }

    bool valid_action(const State& state, const Action& action) const {
        if (action.move_dir==state.second ||
            action.move_dir==opposite(state.second)) {
            return false;
        }
        auto dst = state.first + action.stride*dir2vec(action.move_dir);
        return 0<=dst.first && dst.first<ncols &&
               0<=dst.second && dst.second<nrows;

    }

    State get_next_state(State state, const Action& action) const {
        if (!valid_action(state, action)) {
            return {{-1,-1}, {}};
        }
        state.first = state.first + action.stride*dir2vec(action.move_dir);
        state.second = action.move_dir;
        return state;
    }

    int transition_cost(const State& state, const Action& action) const {
        Vec2i curr_pos = state.first;
        int cost = 0;
        auto move_vec = dir2vec(action.move_dir);
        for (int step = 0; step < action.stride; ++step) {
            curr_pos = curr_pos + move_vec;
            cost += at(curr_pos) - '0';
        }
        return cost;
    }

    vector<tuple<Action,State,int>> get_available_actions(const State& state) const {
        vector<tuple<Action,State,int>> ret;
        for (Direction dir : all_directions) {
            for (int stride = 4; stride <= 10; ++stride) {
                Action action = {dir, stride};
                auto next_state = get_next_state(state, action);
                if (next_state.first.first >= 0) {
                    ret.emplace_back(action, next_state, transition_cost(state, action)); 
                }
            }
        }
        return ret;
    }
};



struct Plan {
    vector<Action> actions;
    int cost;
};

struct VisitedNode {
    Action previous_action;
    State parent;
    int g;
    bool closed;

};

struct OpenNode {
    int f;
    int h;
    State state;
};

struct OpenCmp {
    bool operator()(const OpenNode& lhs, const OpenNode& rhs) const noexcept {
        return lhs.f>rhs.f || (lhs.f==rhs.f && lhs.h>rhs.h);
    }
};

int manhattan_dist(const Vec2i& coords, const Vec2i& target) {
    int dx = target.first - coords.first;
    int dy = target.second - coords.second;
    return abs(dx) + abs(dy);
}

void reconstruct_path(
        vector<Action>& actions, const unordered_map<State,VisitedNode,StateHasher>& visited,
        const State& start, State current) {
    while (current != start) {
        const auto& node = visited.find(current)->second;
        actions.push_back(node.previous_action);
        current = node.parent;
    }
    reverse(actions.begin(), actions.end());
}

ostream& operator<<(ostream& out, const State& state) {
    return out << state.first << ", " << state.second;
}

Plan astar(const Map& map, const State& start, const Vec2i& goal) {
    Plan plan;
    plan.cost = -1;
    unordered_map<State,VisitedNode,StateHasher> visited;
    priority_queue<OpenNode, vector<OpenNode>, OpenCmp> open;
    int heur_start = manhattan_dist(start.first, goal);
    open.push(OpenNode{heur_start, heur_start, start});
    visited[start] = VisitedNode{{}, {}, 0, false};
    while (!open.empty()) {
        auto open_node = open.top();
        open.pop();
        auto& visited_node = visited.find(open_node.state)->second;
        if (visited_node.closed) {
            continue;
        }
        visited_node.closed = true;
        if (open_node.state.first == goal) {
            // plan found, reconstruct path
            reconstruct_path(plan.actions, visited, start, open_node.state);
            plan.cost = visited_node.g;
            break;
        }
        for (auto&&[action,neighbor,cost] : map.get_available_actions(open_node.state)) {
            auto& neigh_visited_node = visited.emplace(neighbor,
                    VisitedNode{{}, {}, INT_MAX, false}).first->second;
            int tentative_g = visited_node.g + cost;
            if (tentative_g < neigh_visited_node.g) {
                neigh_visited_node.g = tentative_g;
                neigh_visited_node.previous_action = action;
                neigh_visited_node.parent = open_node.state;
                int heur_neighbor = manhattan_dist(neighbor.first, goal);
                open.push(OpenNode{tentative_g+heur_neighbor, heur_neighbor, neighbor});
            }
        }
    }
    return plan;
}

ostream& operator<<(ostream& out, const Action& action) {
    out << action.move_dir;
    if (action.stride > 1) {
        out << " x" << action.stride;
    }
    return out;
}

ostream& operator<<(ostream& out, const Plan& plan) {
    out << '[';
    for (size_t i = 0; i < plan.actions.size(); ++i) {
        if (i > 0) {
            out << ", ";
        }
        out << plan.actions[i];
    }
    out << "], cost = " << plan.cost;
    return out;
}

ostream& operator<<(ostream& out, const Map& map) {
    for (int y = 0; y < map.nrows; ++y) {
        for (int x = 0; x < map.ncols; ++x) {
            out << map.at(Vec2i(x,y));
        }
        out << '\n';
    }
    return out;
}

int main() {
    Map map;
    map.read(cin);
    auto plan = astar(map, {{0,0},Direction::north}, {map.ncols-1, map.nrows-1});
    cout << plan << endl;
}
