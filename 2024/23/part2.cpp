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

        int getNodeDegree(int i) const
        {
            return nodes[i].connectedTo.size();
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

bool nextCombination(std::vector<bool>& combination)
{
    size_t i = 0;

    bool wrapAround;

    while (i < combination.size())
    {
        if (combination[i] && !combination[i+1])
        {
            combination[i+1] = true;
            combination[i] = false;
            break;
        }
        ++i;
    }

    wrapAround = i == combination.size()-1;

    size_t j = 0;
    while (true)
    {
        while (j < i && !combination[i])
        {
            --i;
        }

        while (j < i && combination[j])
        {
            ++j;
        }
        if (j < i)
        {
            combination[i] = false;
            combination[j] = true;
        }
        else
        {
            break;
        }
    }

    return !wrapAround;

}

void displayCombination(const std::vector<bool>& combination)
{
    for (bool b : combination)
    {
        std::cout << b;
    }
    std::cout << std::endl;
}

std::vector<bool> newCombination(size_t n, size_t k)
{
    std::vector<bool> combination(n);
    for (size_t i = 0; i < k; ++i)
    {
        combination[i] = true;
    }
    return combination;
}

bool isClique(const Graph& graph, const std::vector<int>& nodes)
{
    for (size_t i = 0; i < nodes.size()-1; ++i)
    {
        for (size_t j = i+1; j < nodes.size(); ++j)
        {
            if (!graph.hasEdge(nodes[i],nodes[j]))
            {
                return false;
            }
        }
    }
    return true;
}

std::vector<int> filter(int u, const std::vector<int>& nodes, const std::vector<bool> filterOut)
{
    std::vector<int> filtered;
    if (!filterOut[u])
    {
        filtered.push_back(u);
    }
    for (int v : nodes)
    {
        if (!filterOut[v])
        {
            filtered.push_back(v);
        }
    }
    return filtered;
}

std::vector<int> subset(const std::vector<int>& nodes, const std::vector<bool>& mask)
{
    std::vector<int> result;
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        if (mask[i])
        {
            result.push_back(nodes[i]);
        }
    }
    return result;
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

    std::vector<bool> filterOut(graph.getNumberOfNodes());

    int maxCliqueSize = 0;
    std::vector<int> largestClique;

    for (int u = 0; u < graph.getNumberOfNodes(); ++u)
    {
        auto cliqueCandidates = filter(u, graph.connectedNodes(u), filterOut);
        for (int k = std::max(maxCliqueSize+1, 3); k < (int)cliqueCandidates.size(); ++k)
        {
            auto combination = newCombination(cliqueCandidates.size(), std::max(maxCliqueSize+1, 3));
            do
            {
                auto nodes = subset(cliqueCandidates, combination);
                if (isClique(graph, nodes))
                {
                    maxCliqueSize = k;
                    largestClique = nodes;
                    break;
                }
            } while (nextCombination(combination));
        }
        filterOut[u] = true; 
    }

    std::vector<std::string> cliqueLabels;
    for (int u : largestClique)
    {
        cliqueLabels.push_back(graph.getNodeLabel(u));
    }

    std::sort(cliqueLabels.begin(), cliqueLabels.end());

    bool first = true;
    for (const std::string& label : cliqueLabels)
    {
        if (!first)
        {
            std::cout << ',';
        }
        first = false;
        std::cout << label;
    }
    std::cout << std::endl;

    std::cout << maxCliqueSize << std::endl;
}
