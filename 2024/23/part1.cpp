#include <algorithm>
#include <iostream>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

class Graph
{
    public:

        void addEdge(const std::string& uLabel, const std::string& vLabel)
        {
            int u = getOrCreateNode(uLabel);
            int v = getOrCreateNode(vLabel);
            nodes[u].connectedTo.push_back(v);
            nodes[v].connectedTo.push_back(u);
        }

        int getOrCreateNode(const std::string& label)
        {
            auto[it,inserted] = labelToId.emplace(label, nodes.size());
            if (inserted)
            {
                nodes.push_back({label});
            }
            return it->second;
        }

        void dot(std::ostream& out)
        {
            out << "graph G {\n";
            for (const auto& node : nodes)
            {
                for (int vId : node.connectedTo)
                {
                    if (node.label < nodes[vId].label)
                    {
                        out << node.label << " -- " << nodes[vId].label << '\n';
                    }
                }
            }
            out << '}';
        }

        bool hasNode(const std::string& label) const
        {
            return labelToId.find(label) != labelToId.end();
        }

        bool hasEdge(int u, int v) const
        {
            return std::find(
                    nodes[u].connectedTo.begin(),
                    nodes[u].connectedTo.end(),
                    v) != nodes[u].connectedTo.end();
        }

        int getNumberOfNodes() const
        {
            return nodes.size();
        }

        const std::string& getNodeLabel(int i) const
        {
            return nodes[i].label;
        }

        const std::vector<int> connectedNodes(int i) const
        {
            return nodes[i].connectedTo;
        }

    private:

        struct NodeInfo
        {
            std::string label;
            std::vector<int> connectedTo;
        };

        std::vector<NodeInfo> nodes;
        std::unordered_map<std::string,int> labelToId;
};

int cliquesSize3(const Graph& graph)
{
    int ans = 0;
    for (int i = 0; i < graph.getNumberOfNodes(); ++i)
    {
        const auto& iLabel = graph.getNodeLabel(i);
        if (iLabel.front() != 't')
        {
            continue;
        }
        const auto& connectedTo = graph.connectedNodes(i);
        for (int j = 0; j < (int)connectedTo.size()-1; ++j)
        {
            const auto& jLabel = graph.getNodeLabel(connectedTo[j]);
            if (jLabel.front() == 't' && jLabel < iLabel)
            {
                continue;
            }
            for (int k = j+1; k < (int)connectedTo.size(); ++k)
            {
                const auto& kLabel = graph.getNodeLabel(connectedTo[k]);
                if (kLabel.front() == 't' && kLabel < iLabel)
                {
                    continue;
                }
                ans += graph.hasEdge(connectedTo[j], connectedTo[k]);
            }
        }
    }
    return ans;
}

int main()
{
    Graph graph;
    std::string line;
    while (std::getline(std::cin, line))
    {
        std::string uLabel = line.substr(0, 2);
        std::string vLabel = line.substr(3, 2);
        graph.addEdge(uLabel, vLabel);
    }

    std::cout << cliquesSize3(graph) << std::endl;
}
