#include <algorithm>
#include <cassert>
#include <iostream>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

enum class GateType { Input, Or, And, Xor };

std::ostream& operator<<(std::ostream& out, GateType gateType)
{
    switch (gateType)
    {
        case GateType::Input:
            out << "Input";
            break;
        case GateType::Or:
            out << "Or";
            break;
        case GateType::And:
            out << "And";
            break;
        case GateType::Xor:
            out << "Xor";
            break;
        default:
            assert(false);
    }
    return out;
}

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
        
        out << "rank=TD\n";
        
        std::vector<std::string> inputsX;
        std::vector<std::string> inputsY;
        std::vector<std::string> outputs;
        std::vector<std::string> intermediateNodes;
        
        for (const auto&[gateName,_] : gates)
        {
            switch (gateName.front())
            {
                case 'x':
                    inputsX.push_back(gateName);
                    break;
                case 'y':
                    inputsY.push_back(gateName);
                    break;
                case 'z':
                    outputs.push_back(gateName);
                    break;
                default:
                    intermediateNodes.push_back(gateName);
            }
        }
        
        std::sort(inputsX.begin(), inputsX.end());
        std::sort(inputsY.begin(), inputsY.end());
        std::sort(outputs.begin(), outputs.end());
        
        // out << "subgraph cluster_InputsX {\n";
        
        // out << "edge[style=invis]\n";
        
        // for (size_t i = 0; i < inputsX.size()-1; ++i)
        // {
            // out << inputsX[i] << " -> " << inputsX[i+1] << '\n';
        // }
        
        // out << "label=\"X inputs\"\n";
        
        // out << "}\n";
        
        // out << "subgraph cluster_InputsY {\n";
        
        // out << "edge[style=invis]\n";
        
        // for (size_t i = 0; i < inputsY.size()-1; ++i)
        // {
            // out << inputsY[i] << " -> " << inputsY[i+1] << '\n';
        // }
        
        // out << "label=\"Y inputs\"\n";
        
        // out << "}\n";
        
        // out << "subgraph cluster_Outputs {\n";
        
        for (const auto& gateName : outputs)
        {
            const auto& gate = gates.find(gateName)->second;
            out << gateName << "[label = < " << gateName << "<br/><b>" << gate.type << "</b> >,shape=box,style=filled,color=lightgrey]\n";
        }
        
        // out << "edge[style=invis]\n";
        
        // for (size_t i = 0; i < outputs.size()-1; ++i)
        // {
            // out << outputs[i] << " -> " << outputs[i+1] << '\n';
        // }
        
        // out << "label=\"Outputs\"\n";
        
        // out << "}\n";
        
        for (const auto& gateName : intermediateNodes)
        {
            const auto& gate = gates.find(gateName)->second;
            out << gateName << "[label = < " << gateName << "<br/><b>" << gate.type << "</b> >,shape=box]\n";
        }
        
        for (const auto& gateName : intermediateNodes)
        {
            const auto& gate = gates.find(gateName)->second;
            out << gate.input1 << " -> " << gateName << "\n";
            out << gate.input2 << " -> " << gateName << "\n";
        }
        
        for (const auto& gateName : outputs)
        {
            const auto& gate = gates.find(gateName)->second;
            out << gate.input1 << " -> " << gateName << "\n";
            out << gate.input2 << " -> " << gateName << "\n";
        }
        
        out << "}\n";
    }
};

int main()
{
    // LogicCircuit logicCircuit;
    // std::string line;
    // while (std::getline(std::cin, line) && !line.empty()) 
    // {
        // logicCircuit.addInput(line);
    // }
    // while (std::getline(std::cin, line)) 
    // {
        // logicCircuit.addGate(line);
    // }
    
    // // swaps found by visual inspection
    // std::swap(logicCircuit.gates["z09"], logicCircuit.gates["gwh"]);
    // std::swap(logicCircuit.gates["z21"], logicCircuit.gates["rcb"]);
    // std::swap(logicCircuit.gates["z39"], logicCircuit.gates["jct"]);
    // std::swap(logicCircuit.gates["wbw"], logicCircuit.gates["wgb"]);

    // logicCircuit.dot(std::cout);
    
    std::vector<std::string> ans{"z09","gwh","z21","rcb","z39","jct","wbw","wgb"};
    std::sort(ans.begin(), ans.end());
    std::cout << ans[0];
    for (size_t i = 1; i < ans.size(); ++i)
    {
        std::cout << ',' << ans[i];
    }
    std::cout << std::endl;
}
