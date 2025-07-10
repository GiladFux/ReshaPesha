#include "elf_text_dump.h"
#include <iostream> 
#include <fstream>

int main(int argc, char** argv) {
    std::string path = "test_elf_32";
    
    if (!dump_elf_32(path.c_str())) {
        std::cerr << "Failed to dump ELF file" << std::endl;
        return 1;
    }

    return 0;
}
