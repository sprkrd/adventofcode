#include <algorithm>
#include <cassert>
#include <iostream>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

enum class GateType { Input, Or, And, Xor };

struct Gate
{
    std::string input1;
    std::string input2;
    GateType type;
    bool valueKnown;
    bool value;
};

std::vector<std::string> split(const std::string& str, const std::string& delim=" \n\t") {
    std::vector<std::string> parts;
    size_t pos = 0;
    while (pos < str.length())
    {
        size_t pos_next = str.find_first_of(delim, pos);
        if (pos_next > pos)
        {
            parts.push_back(str.substr(pos, pos_next-pos));
        }
        pos = pos_next == std::string::npos? std::string::npos : pos_next + 1;
    }
    return parts;
}

struct LogicCircuit
{
    std::unordered_map<std::string, Gate> gates;

    bool eval(const std::string& gateName)
    {
        auto& gate = gates.find(gateName)->second;
        if (!gate.valueKnown)
        {
            bool eInput1 = eval(gate.input1);
            bool eInput2 = eval(gate.input2);
            switch (gate.type)
            {
                case GateType::Or:
                    gate.value = eInput1 || eInput2;
                    break;
                case GateType::And:
                    gate.value = eInput1 && eInput2;
                    break;
                case GateType::Xor:
                    gate.value = eInput1 ^ eInput2;
                    break;
                default:
                    assert(false);
            }
            gate.valueKnown = true;
        }
        return gate.value;
    }

    void addInput(const std::string& line)
    {
        auto parts = split(line, ": ");
        gates.emplace(parts[0], Gate{"", "", GateType::Input, true,
                      (bool)std::stoi(parts[1])});
    }

    void addGate(const std::string& line)
    {
        auto parts = split(line, " ->");
        GateType gateType;
        if (parts[1] == "OR")
        {
            gateType = GateType::Or;
        }
        else if (parts[1] == "AND")
        {
            gateType = GateType::And;
        }
        else if (parts[1] == "XOR")
        {
            gateType = GateType::Xor;
        }
        else
        {
            assert(false);
        }
        gates.emplace(parts[3], Gate{parts[0], parts[2], gateType, false, false});
    }
    
    void dot(std::ostream& out) const
    {
        out << "digraph G {\n";
        
        std::vector<std::string> inputs;
        std::vector<std::string> outputs;
        std::vector<std::string> intermediateNodes;
        
        for (const auto&[gateName,_] : gates)
        {
            switch (gateName.front())
            {
                case 'x':
                case 'y':
                    inputs.push_back(gateName);
                    break;
                case 'z':
                    outputs.push_back(gateName);
                    break;
                default:
                    intermediateNodes.push_back(gateName);
            }
        }
        
        std::sort(inputs.begin(), inputs.end(), [](const std::string& i1, const std::string& i2)
        {
            return i1.substr(1)+i1[0] < i2.substr(1)+i2[0];
        });
        std::sort(outputs.begin(), outputs.end());
        
        out << "subgraph cluster_Inputs {\n";
        
        out << "edge[style=invis]\n";
        
        for (size_t i = 0; i < inputs.size()-1; ++i)
        {
            out << inputs[i] << " -> " << inputs[i+1] << '\n';
        }
        
        out << "label=\"Inputs\"\n";
        
        out << "}\n";
        
        out << "subgraph cluster_Outputs {\n";
        
        out << "edge[style=invis]\n";
        
        for (size_t i = 0; i < outputs.size()-1; ++i)
        {
            out << outputs[i] << " -> " << outputs[i+1] << '\n';
        }
        
        out << "label=\"Outputs\"\n";
        
        out << "}\n";
        
        out << "}\n";
    }
};

int main()
{
    LogicCircuit logicCircuit;
    std::string line;
    while (std::getline(std::cin, line) && !line.empty()) 
    {
        logicCircuit.addInput(line);
    }
    while (std::getline(std::cin, line)) 
    {
        logicCircuit.addGate(line);
    }

    logicCircuit.dot(std::cout);
}
