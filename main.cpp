#include "elf_parser.hpp"
#include "disassembler.hpp"
#include <iostream>
#include <iomanip>

int main(int argc, char** argv) {
    std::string path = "test_elf_32";

    Elf32Parser parser(path);
    if (!parser.parse()) {
        std::cerr << "Failed to parse ELF file: " << parser.getError() << std::endl;
        return 1;
    }

    std::vector<unsigned char> text_section = parser.getSectionData(".text");
    if (text_section.empty()) {
        std::cerr << "Could not find .text section" << std::endl;
        return 1;
    }

    uint32_t text_addr = parser.getSectionAddr(".text");
    uint32_t main_addr = parser.getSymbolAddr("main");

    auto instructions = disassemble_x86(text_section, text_addr);

    std::cout << "Disassembly (.text section):" << std::endl;
    for (const auto& instr : instructions) {
        if (instr.address == main_addr) {
            std::cout << ">>> MAIN FUNCTION START <<<" << std::endl;
        }
        std::cout << std::hex << instr.address << ": "
                  << instr.mnemonic << " " << instr.op_str << std::endl;
    }

    return 0;
}
