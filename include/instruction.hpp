#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <string>
#include <vector>
#include <cstdint>

class Instruction {
public:
    uint32_t address;              // Memory address of the instruction
    std::vector<uint8_t> bytes;    // Raw bytes of the instruction
    std::string mnemonic;          // Instruction mnemonic (e.g., "mov", "add")
    std::string op_str;            // Operand string (e.g., "eax, ebx")


    Instruction(uint32_t addr = 0, const std::vector<uint8_t>& b = {},
                const std::string& mn = "", const std::string& op = "")
        : address(addr), bytes(b), mnemonic(mn), op_str(op) {}

    std::string toString() const;
};

#endif // INSTRUCTION_HPP
