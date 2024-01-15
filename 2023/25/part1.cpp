#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

class FlowNetwork {
    public:

        int add_or_get_node(const string& label) {
            auto[it,inserted] = node_to_index.emplace(label, nodes.size());
            if (inserted) {
                nodes.push_back(label);
                outgoing_edges.emplace_back();
            }
            return it->second;
        }

        void add_or_set_capacity(const string& src, const string& dst, int capacity) {
            int src_idx = add_or_get_node(src);
            int dst_idx = add_or_get_node(dst);
            auto[it,inserted] = edges.try_emplace(Edge(src_idx, dst_idx), capacity);
            if (inserted) {
                auto it_reverse = edges.try_emplace(Edge(dst_idx, src_idx), 0).first;
                outgoing_edges[src_idx].push_back(it);
                outgoing_edges[dst_idx].push_back(it_reverse);
            }
            else {
                it->second.capacity = capacity;
            }
        }

        void dot(ostream& out) const {
            out << "digraph G {\n";
            for (const auto& label : nodes) {
                out << label << '\n';
            }

            for (const auto&[edge,edge_info] : edges) {
                out << nodes[edge.src] << " -> " << nodes[edge.dst] << " [label=\""
                    << edge_info.flow << '/' << edge_info.capacity << "\"]\n";
            }
            out << "}\n";
        }

        void reset_flow() {
            for (auto&[_,edge_info] : edges) {
                edge_info.flow = 0;
            }
        }

        pair<vector<int>,int> find_augmenting_path(const string& src, const string& dst) const {
            int src_idx = node_to_index.find(src)->second;
            int dst_idx = node_to_index.find(dst)->second;

            vector<int> augmenting_path;
            vector<bool> marked(nodes.size());
            vector<tuple<int,int,bool>> stk{{src_idx, numeric_limits<int>::max(), false}};
            while (!stk.empty()) {
                auto[node_idx,inc_flow,expanded] = stk.back();
                stk.pop_back();
                if (expanded) {
                    augmenting_path.pop_back();
                    continue;
                }
                if (marked[node_idx]) {
                    continue;
                }
                marked[node_idx] = true;
                augmenting_path.push_back(node_idx);
                if (node_idx == dst_idx) {
                    return {augmenting_path,inc_flow};
                }
                stk.emplace_back(node_idx,inc_flow,true);
                for (const auto& it : outgoing_edges[node_idx]) {
                    int next = it->first.dst;
                    int res = it->second.residual();
                    if (res>0) {
                        stk.emplace_back(next,min(inc_flow,res),false);
                    }
                }
            }
            return {augmenting_path, 0};
        }

        void augment(const vector<int>& path, int inc_flow) {
            for (size_t i = 1; i < path.size(); ++i) {
                Edge direct_edge(path[i-1], path[i]);
                Edge reverse_edge(path[i], path[i-1]);
                edges.find(direct_edge)->second.flow += inc_flow;
                edges.find(reverse_edge)->second.flow -= inc_flow;
            }
        }

        const vector<string>& get_nodes() const {
            return nodes;
        }

        vector<string> get_reachable_nodes(const string& seed) {
            int seed_idx = node_to_index.find(seed)->second;
            vector<bool> marked(nodes.size());
            vector<int> reachable;
            vector<int> stk{seed_idx};
            while (!stk.empty()) {
                int node_idx = stk.back();
                stk.pop_back();
                if (marked[node_idx]) {
                    continue;
                }
                marked[node_idx] = true;
                reachable.push_back(node_idx);
                for (const auto& it : outgoing_edges[node_idx]) {
                    int next = it->first.dst;
                    int res = it->second.residual();
                    if (res > 0) {
                        stk.push_back(next);
                    }
                }
            }
            vector<string> reachable_nodes(reachable.size());
            for (size_t i = 0; i < reachable.size(); ++i) {
                reachable_nodes[i] = nodes[reachable[i]];
            }
            return reachable_nodes;
        }

    private:

        struct Edge {
            int src;
            int dst;

            Edge(int src, int dst) : src(src), dst(dst) {}

            bool operator==(const Edge& other) const {
                return src==other.src && dst==other.dst;
            }
        };

        struct EdgeHasher {
            size_t operator()(const Edge& edge) const noexcept {
                return edge.src*123'456'791ULL + edge.dst;
            }
        };

        struct EdgeInfo {
            int flow;
            int capacity;

            EdgeInfo(int capacity) : flow(0), capacity(capacity) {}

            int residual() const {
                return capacity - flow;
            }
        };

        typedef unordered_map<Edge,EdgeInfo,EdgeHasher> EdgeMap;

        vector<string> nodes;
        unordered_map<string,int> node_to_index;

        EdgeMap edges;
        vector<vector<EdgeMap::iterator>> outgoing_edges;
};

int calculate_max_flow(FlowNetwork& network, const string& src, const string& dst) {
    int max_flow = 0;
    vector<int> augmenting_path;
    int inc_flow;
    tie(augmenting_path, inc_flow) = network.find_augmenting_path(src, dst);
    while (inc_flow) {
        max_flow += inc_flow;
        network.augment(augmenting_path, inc_flow);
        tie(augmenting_path, inc_flow) = network.find_augmenting_path(src, dst);
    }
    return max_flow;
}

void read_network(istream& in, FlowNetwork& network) {
    string line;
    while (getline(in,line)) {
        istringstream iss(line);
        string src, dst;
        iss >> src;
        src.pop_back();
        while (iss >> dst) {
            network.add_or_set_capacity(src, dst, 1);
            network.add_or_set_capacity(dst, src, 1);
        }
    }
}

int main() {
    FlowNetwork network;
    read_network(cin, network);

    mt19937 rng{random_device{}()};

    const auto& nodes = network.get_nodes();
    uniform_int_distribution<int> unif(0, nodes.size()-1);

    string src;
    string dst;

    while (true) {
        src = nodes[unif(rng)];
        dst = nodes[unif(rng)];
        int max_flow = calculate_max_flow(network, src, dst);
        if (max_flow == 3) {
            break;
        }
        network.reset_flow();
    }

    auto reachable_from_src = network.get_reachable_nodes(src);

    size_t size_set1 = reachable_from_src.size();
    size_t size_set2 = nodes.size() - reachable_from_src.size();

    size_t ans = size_set1 * size_set2;

    cout << ans << endl;
}
