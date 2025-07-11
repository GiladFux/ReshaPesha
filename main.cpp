#include "elf_parser.hpp"
#include <iostream>
#include <iomanip>

int main(int argc, char** argv) {
    std::string path = "test_elf_32";

    Elf32Parser parser(path);
    if (!parser.parse()) {
        std::cerr << "Failed to parse ELF file: " << parser.getError() << std::endl;
        return 1;
    }

    std::vector<uint8_t> text_section = parser.getSectionData(".text");
    if (text_section.empty()) {
        std::cerr << "Could not find .text section" << std::endl;
        return 1;
    }

    std::cout << ".text section contents (hex):" << std::endl;
    for (size_t i = 0; i < text_section.size(); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(text_section[i]) << " ";
        if ((i + 1) % 16 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;

    return 0;
}
