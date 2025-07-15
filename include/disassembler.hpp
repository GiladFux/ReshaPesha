#ifndef DISASSEMBLER_HPP
#define DISASSEMBLER_HPP

#include "instruction.hpp"
#include <vector>
#include <string>
#include <cstdint>

std::string reg32_name(unsigned char reg);
std::vector<Instruction> disassemble_x86(const std::vector<unsigned char>& code, uint32_t base_addr);

#endif