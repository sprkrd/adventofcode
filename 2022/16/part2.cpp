#include "utils.hpp"

#include <cassert>
#include <iostream>
#include <queue>
#include <unordered_map>
using namespace std;

typedef pair<int,int> Weighted_edge;

struct Valve {
    string label;
    int id;
    int rate;
    vector<Weighted_edge> leads_to;
};

struct Graph {
    unordered_map<string,int> tr;
    vector<Valve> data;

    static Graph load(istream& in) {
        Graph graph;
        string line;
        while (getline(cin,line)) {
            auto valves = ash::every_valve(line);
            Valve& valve = graph.node(valves[0]);
            valve.rate = ash::find_rate(line);
            for (size_t i = 1; i < valves.size(); ++i) {
                graph.edge(valves[0], valves[i]);
            }
        }
        return graph;
    }

    Valve& node(const string& label) {
        auto it = tr.find(label);
        if (it == tr.end()) {
            int id = tr.size();
            it = tr.emplace(label, id).first;
            data.emplace_back(Valve{label,id,0,{}});
        }
        return data[it->second];
    }

    Weighted_edge& edge(const string& from, const string& to, int weight = 1) {
        int from_id = node(from).id;
        int to_id = node(to).id;
        data[from_id].leads_to.emplace_back(weight, to_id);
        return data[from_id].leads_to.back();
    }

    int nodes() const {
        return data.size();
    }

    const Valve& at(const string& label) const {
        return data[tr.at(label)];
    }

    const Valve& operator[](int id) const {
        return data[id];
    }

    vector<Weighted_edge> get_reachable(int start) {
        const int inf = numeric_limits<int>::max();
        queue<int> open;
        open.push(start);
        vector<int> distance_to(nodes(), inf);
        distance_to[start] = 0;
        while (!open.empty()) {
            int id = open.front();
            open.pop();
            int level = distance_to[id];
            for (auto&&[weight,neighbor_id] : (*this)[id].leads_to) {
                assert(weight == 1);
                if (distance_to[neighbor_id] != inf)
                    continue;
                distance_to[neighbor_id] = level + 1;
                if ((*this)[neighbor_id].rate == 0)
                    open.push(neighbor_id);
            }
        }
        vector<Weighted_edge> reachable;
        for (int i = 0; i < nodes(); ++i) {
            if (i != start && distance_to[i] != inf && (*this)[i].rate != 0) {
                reachable.emplace_back(distance_to[i], i);
            }
        }
        return reachable;
    }


    void simplify(int initial_valve) {
        Graph simplified;
        for (const Valve& valve : data) {
            if (valve.rate != 0 || valve.id == initial_valve) {
                Valve& valve_new = simplified.node(valve.label);
                valve_new.rate = valve.rate;
                for (auto[weight,other_id] : get_reachable(valve.id)) {
                    simplified.edge(valve.label, (*this)[other_id].label, weight);
                }
            }
        }
        tr = move(simplified.tr);
        data = move(simplified.data);
    }
};

ostream& operator<<(ostream& out, const Graph& graph) {
    for (const auto& valve : graph.data) {
        out << "Valve " << valve.label << " has flow rate=" << valve.rate << "; "
            << "tunnels lead to valves ";
        bool first = true;
        for (auto[weight, other_id] : valve.leads_to) {
            if (!first) out << ", ";
            const Valve& other = graph[other_id];
            out << '(' << weight << ',' << other.label << ')';
            first = false;
        }
        out << '\n';
    }
    return out;
}

struct Optimize {
    typedef tuple<int,int,uint64_t> Args;
    typedef size_t (*Args_hash)(const Args&);

    const Graph& graph;
    unordered_map<Args,int,Args_hash> cache;
    int non_zero_rate;

    static size_t hash_args(const Args& args) {
        auto[minutes_remaining,valve_num,open_valves] = args;
        size_t seed = open_valves;
        seed = seed*61 + valve_num;
        seed = seed*30 + minutes_remaining;
        return seed;
    }

    Optimize(const Graph& graph) :  graph(graph),
                                    cache(1000, Optimize::hash_args) {
    }

    int operator()(int minutes_remaining, const string& valve_id, uint64_t available_valves = -1) {
        uint64_t open_valves = ((1ULL<<graph.nodes())-1)&~available_valves; 
        return (*this)(minutes_remaining, graph.at(valve_id).id, open_valves);
    }

    int operator()(int minutes_remaining, int id, uint64_t open_valves) {
        if (minutes_remaining <= 0) {
            return 0;
        }
        Args args(minutes_remaining, id, open_valves);
        auto it = cache.find(args);
        if (it == cache.end()) {
            int max_released = 0;
            const Valve& valve = graph[id];
            bool current_valve_open = (open_valves>>id)&1;

            if (!current_valve_open && valve.rate != 0) {
                // open the current valve
                uint64_t new_open_mask = open_valves | (1ULL<<id);
                int released = (minutes_remaining-1)*valve.rate +
                    (*this)(minutes_remaining-1, id, new_open_mask);
                max_released = max(max_released, released);
            }

            for (auto[travel_time,neigh_id] : valve.leads_to) {
                int released = (*this)(minutes_remaining-travel_time, neigh_id, open_valves);
                max_released = max(max_released, released);
            }
            bool ignored;
            tie(it, ignored) = cache.emplace(args, max_released);
        }
        return it->second;
    }
};

int main() {
    auto graph = Graph::load(cin);
    graph.simplify(graph.at("AA").id);
    cout << graph << endl;

    Optimize optimize(graph);

    int opt = 0;
    for (uint64_t available_valves = 0; available_valves < (1ULL<<graph.nodes()); ++available_valves) {
        int pressure = optimize(26, "AA", available_valves);
        pressure += optimize(26, "AA", ~available_valves);
        opt = max(opt, pressure);
    }

    cout << opt << endl;

    //cout << optimal_pressure(30, tr.at("AA")) << endl;
}
