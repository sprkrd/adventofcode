#include <iostream>
#include <unordered_map>
#include "utils.hpp"

constexpr int number_of_resources = 3;

typedef std::array<int,3> Array_3;

constexpr int done = -2;
constexpr int wait = -1;
constexpr int ore = 0;
constexpr int clay = 1;
constexpr int obsidian = 2;
constexpr int geode = 3;

struct Blueprint {
    int index;
    std::array<Array_3,4> cost_matrix;

    void read(const std::string& blueprint_text) {
        for (auto& row : cost_matrix) {
            row.fill(0);
        }
        auto ints = ash::every_int(blueprint_text);
        index = ints[0];
        cost_matrix[ore][ore] = ints[1];
        cost_matrix[clay][ore] = ints[2];
        cost_matrix[obsidian][ore] = ints[3];
        cost_matrix[obsidian][clay] = ints[4];
        cost_matrix[geode][ore] = ints[5];
        cost_matrix[geode][obsidian] = ints[6];
    }
};

std::ostream& operator<<(std::ostream& out, const Blueprint& blueprint) {
    out << "Blueprint " << blueprint.index << ": "
        << "Each ore robot costs " << blueprint.cost_matrix[ore][ore] << " ore. "
        << "Each clay robot costs " << blueprint.cost_matrix[clay][ore] << " ore. "
        << "Each obsidian robot costs " << blueprint.cost_matrix[obsidian][ore] << " ore and "
                                        << blueprint.cost_matrix[obsidian][clay] << " clay. "
        << "Each geode robot costs " << blueprint.cost_matrix[geode][ore] << " ore and "
                                     << blueprint.cost_matrix[geode][obsidian] << " obsidian.";
    return out;
}

struct State {
    int minutes_remaining;
    Array_3 production;
    Array_3 resources;

    State(int minutes_remaining) : minutes_remaining(minutes_remaining), production{1,0,0}, resources{0,0,0} {
    }

    int action_wait_time(const Blueprint& blueprint, int action) const {
        if (action == wait) {
            return 1;
        }
        if (action == done) {
            return minutes_remaining;
        }
        int wait_time = 0;
        for (int resource = 0; resource < number_of_resources; ++resource) {
            int resource_wait_time;
            if (blueprint.cost_matrix[action][resource] == 0) {
                resource_wait_time = 0;
            }
            else if (production[resource] == 0 &&
                     resources[resource] < blueprint.cost_matrix[action][resource]) {
                resource_wait_time = std::numeric_limits<int>::max();
            }
            else {
                resource_wait_time = blueprint.cost_matrix[action][resource] - resources[resource];
                resource_wait_time = resource_wait_time + production[resource] - 1;
                resource_wait_time = resource_wait_time / production[resource];
                resource_wait_time = std::max(0, resource_wait_time) + 1;
            }
            wait_time = std::max(wait_time, resource_wait_time);
        }
        return wait_time;
    }

    std::vector<int> available_actions(const Blueprint& blueprint) const {
        std::vector<int> actions;
        for (int action = 0; action <= geode; ++action) {
            bool useful_action = true;
            if (action != geode) {
                useful_action = false;
                for (int resource = 0; resource <= geode && !useful_action; ++resource) {
                    if (production[action] < blueprint.cost_matrix[resource][action]) {
                        useful_action = true;
                    }
                }
            }
            if (!useful_action) {
                continue;
            }
            int wait_time = action_wait_time(blueprint, action);
            if (action == geode && wait_time == 1) {
                actions.clear();
            }
            if (wait_time <= minutes_remaining) {
                actions.push_back(action);
            }
        }
        if (actions.empty()) {
            actions.push_back(done);
        }
        return actions;
    }

    int step(const Blueprint& blueprint, int action) {
        int reward = 0;
        int wait_time = action_wait_time(blueprint, action);

        minutes_remaining -= wait_time;
        for (int resource = 0; resource < number_of_resources; ++resource) {
            resources[resource] += production[resource]*wait_time;
        }

        if (action != wait && action != done) {
            if (action == geode) {
                reward = minutes_remaining;
            }
            else {
                ++production[action];
            }
            for (int resource = 0; resource < number_of_resources; ++resource) {
                resources[resource] -= blueprint.cost_matrix[action][resource];
            }
        }
        
        return reward;
    }

    bool operator==(const State& other) const noexcept {
        return minutes_remaining == other.minutes_remaining &&
               production == other.production &&
               resources == other.resources;
    }
};

std::ostream& operator<<(std::ostream& out, const State& state) {
    constexpr const char* resource_names[4] = {"ore", "clay", "obsidian"};
    out << "<State>" << '\n'
        << "    " << state.minutes_remaining << " minutes remaining.\n";
    for (int resource = 0; resource < number_of_resources; ++resource) {
        out << "    " <<  resource_names[resource] << " (qty/production): "
            << state.resources[resource] << '/' << state.production[resource] << '\n';
    }
    out << "</State>";
    return out;
}

namespace std {

template <>
struct hash<State> {
    size_t operator()(const State& state) const noexcept {
        size_t seed = 0;
        ash::hash_combine(seed, state.minutes_remaining);
        for (int resource : state.resources) {
            ash::hash_combine(seed, resource);
        }
        for (int resource_production : state.production) {
            ash::hash_combine(seed, resource_production);
        }
        return seed;
    }
};

}

struct Optimize {
    typedef std::pair<int, int> Action_value;
    const Blueprint& blueprint;
    std::unordered_map<State,Action_value> cache;

    Optimize(const Blueprint& blueprint) : blueprint(blueprint) {
    }

    Action_value operator()(const State& state) {
        if (state.minutes_remaining == 0) {
            return Action_value(done, 0);
        }
        auto it = cache.find(state);
        if (it != cache.end()) {
            return it->second;
        }
        int best_action = wait;
        int max_value = 0;
        for (int action : state.available_actions(blueprint)) {
            State next = state;
            int immediate_reward = next.step(blueprint, action);
            auto[ignored, delayed_reward] = (*this)(next);
            int accumulated_reward = immediate_reward + delayed_reward;
            if (accumulated_reward > max_value) {
                best_action = action;
                max_value = accumulated_reward;
            }
        }
        std::tie(it,std::ignore) = cache.emplace(state, Action_value(best_action, max_value));
        return it->second;
    }
};

int main() {
    std::vector<Blueprint> blueprints;
    std::string line;
    while (std::getline(std::cin, line) && blueprints.size() < 3) {
        blueprints.emplace_back();
        blueprints.back().read(line);
    }

    int answer = 1;
    
    std::cout << "Processing " << blueprints.size() << " blueprint(s)" << std::endl;

    for (const Blueprint& blueprint : blueprints) {
        std::cout << "Processing blueprint " << blueprint.index << std::endl;
        State state(32);
        Optimize optimize(blueprint);
        auto[ignore,geodes] = optimize(state);
        answer *= geodes;
    }

    std::cout << answer << std::endl;
}
