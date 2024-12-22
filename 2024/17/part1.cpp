#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <optional>

constexpr int kRegisterA = 0;
constexpr int kRegisterB= 1;
constexpr int kRegisterC = 2;

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


struct Instruction
{
    enum Verb { adv = 0, bxl, bst, jnz, bxc, out, bdv, cdv};
    Verb verb;
    int argument;
};

const char* VerbStr[] = { "adv", "bxl", "bst", "jnx", "bxc", "out", "bdv", "cdv" };

std::ostream& operator<<(std::ostream& out, Instruction::Verb verb)
{
    return out << VerbStr[static_cast<int>(verb)];
}

std::ostream& operator<<(std::ostream& out, const Instruction& instruction)
{
    return out << instruction.verb << ' ' << instruction.argument;
}

struct Program
{
    std::vector<Instruction> instructions;
    
    void read(std::istream& in)
    {
        std::string programStr;
        in >> programStr >> programStr;
        auto programParts = split(programStr, ",");
        for (size_t i = 0; i < programParts.size(); i += 2)
        {
            instructions.push_back({
                static_cast<Instruction::Verb>(std::stoi(programParts[i])),
                std::stoi(programParts[i+1])
            });
        }
    }
};

std::ostream& operator<<(std::ostream& out, const Program& program)
{
    bool first = true;
    for (const auto& instruction : program.instructions)
    {
        if (!first)
        {
            out << '\n';
        }
        first = false;
        out << instruction;
    }
    return out;
}

struct Context
{
    std::array<int,3> registers;
    int programCounter = 0;
    
    void read(std::istream& in)
    {
        std::string dummy;
        in >> dummy >> dummy >> registers[kRegisterA]
           >> dummy >> dummy >> registers[kRegisterB]
           >> dummy >> dummy >> registers[kRegisterC];
    }
    
    int getComboOperand(int argument) const
    {
        if (argument <= 3)
        {
            return argument;
        }
        return registers[argument-4];
    }
    
    void stepAdv(int argument)
    {
        registers[kRegisterA] /= 1<<getComboOperand(argument);
        ++programCounter;
    }
    
    void stepBxl(int argument)
    {
        registers[kRegisterB] ^= argument;
        ++programCounter;
    }
    
    void stepBst(int argument)
    {
        registers[kRegisterB] = getComboOperand(argument)%8;
        ++programCounter;
    }
    
    void stepJnz(int argument)
    {
        if (registers[kRegisterA])
        {
            programCounter = argument;
        }
        else
        {
            ++programCounter;
        }
    }
    
    void stepBxc(int /*ignored*/)
    {
        registers[kRegisterB] ^= registers[kRegisterC];
        ++programCounter;
    }
    
    void stepOut(int argument)
    {
        std::cout << (getComboOperand(argument)%8) << ',';
        ++programCounter;
    }
    
    void stepBdv(int argument)
    {
        registers[kRegisterB] = registers[kRegisterA] / (1<<getComboOperand(argument));
        ++programCounter;
    }
    
    void stepCdv(int argument)
    {
        registers[kRegisterC] = registers[kRegisterA] / (1<<getComboOperand(argument));
        ++programCounter;
    }
    
    bool step(const Program& program)
    {
        const auto& instruction = program.instructions[programCounter];
        switch (instruction.verb)
        {
            case Instruction::adv:
                stepAdv(instruction.argument);
                break;
            case Instruction::bxl:
                stepBxl(instruction.argument);
                break;
            case Instruction::bst:
                stepBst(instruction.argument);
                break;
            case Instruction::jnz:
                stepJnz(instruction.argument);
                break;
            case Instruction::bxc:
                stepBxc(instruction.argument);
                break;
            case Instruction::out:
                stepOut(instruction.argument);
                break;
            case Instruction::bdv:
                stepBdv(instruction.argument);
                break;
            default: //case Instruction::Cdv:
                stepCdv(instruction.argument);
        }
        return programCounter >= (int)program.instructions.size();
    }
};


std::ostream& operator<<(std::ostream& out, const Context& context)
{
    out << "Register A: " << context.registers[kRegisterA] << '\n'
        << "Register B: " << context.registers[kRegisterB] << '\n'
        << "Register C: " << context.registers[kRegisterC] << '\n'
        << "Program counter: " << context.programCounter;
    return out;
}

int main()
{
    Context context;
    Program program;
    context.read(std::cin);
    program.read(std::cin);
    
    std::cout << context << std::endl
              << program << std::endl;
              
    while (!context.step(program))
    {
        // std::cout << context << "\n---------" << std::endl;
    }
    std::cout << std::endl;
}
