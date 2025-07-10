#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>

struct Elf32_Header
{
    unsigned char e_ident[16];   // Magic number + class + data encoding + version etc.
    uint16_t type;             // Type of ELF file (executable, shared object...)
    uint16_t arch;          // CPU architecture (x86, ARM, etc.)
    uint32_t version;          // ELF version
    uint32_t entry;            // Entry point address (where the CPU starts executing)
    uint32_t e_phoff;            // Offset in file to program header table
    uint32_t section_offset;            // Offset in file to section header table
    uint32_t flags;            // Processor-specific flags
    uint16_t ehsize;           // ELF header size in bytes
    uint16_t phentsize;        // Size of a program header entry
    uint16_t phnum;            // Number of program header entries
    uint16_t section_size;        // Size of a section header entry
    uint16_t section_num;            // Number of section header entries
    uint16_t section_table_index;         // Section header string table index
};

struct Elf32_SectionHeader 
{
    uint32_t sh_name;       // Offset into the section header string table (name of this section)
    uint32_t sh_type;       // Section type (code, data, symbol table...)
    uint32_t sh_flags;      // Section flags (executable, writable, etc.)
    uint32_t sh_addr;       // Virtual memory address at runtime (not needed here)
    uint32_t sh_offset;     // Offset in the file where this section’s data starts
    uint32_t sh_size;       // Size of this section in bytes
    uint32_t sh_link;       // Link to another section (depends on type)
    uint32_t sh_info;       // Extra info (depends on type)
    uint32_t sh_addralign;  // Alignment requirements
    uint32_t sh_entsize;    // Entry size if section holds fixed-size entries
};


bool dump_elf_32(const std::string& filepath){
    const char* filename = filepath.c_str();

    std::ifstream file(filename, std::ios::binary);

    if (!file) 
    {   
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1;
    }
      
    Elf32_Header elf_header;
    file.read(reinterpret_cast<char*>(&elf_header), sizeof(Elf32_Header));
    if (!file) 
    {
        std::cerr << "Error reading ELF header from file: " << filename << std::endl;
        return 1;
    }
    if (std::memcmp(elf_header.e_ident, "\x7f""ELF", 4) != 0) 
    {
        std::cerr << "Not a valid ELF file: " << filename << std::endl;
        return 1;
    }

    std::cout << "ELF Header Information:" << std::endl;
    std::cout << "  Type: " << elf_header.type << std::endl;
    std::cout << "  Architecture: " << elf_header.arch << std::endl;
    std::cout << "  Entry point address: " << std::hex << elf_header.entry << std::dec << std::endl;
    std::cout << "  Program header offset: " << elf_header.e_phoff << std::endl;
    std::cout << "  Section header offset: " << elf_header.section_offset << std::endl;
    std::cout << "  Flags: " << elf_header.flags << std::endl;      
    std::cout << "  ELF header size: " << elf_header.ehsize << std::endl;
    std::cout << "  Program header entry size: " << elf_header.phentsize << std::endl;
    std::cout << "  Number of program header entries: " << elf_header.phnum << std::endl;
    std::cout << "  Section header entry size: " << elf_header.section_size << std::endl;
    std::cout << "  Number of section header entries: " << elf_header.section_num << std::endl;
    std::cout << "  Section header string table index: " << elf_header.section_table_index << std::endl; 
    file.seekg(elf_header.section_offset);
    std::vector<Elf32_SectionHeader> section_headers(elf_header.section_num);
    file.read(reinterpret_cast<char*>(section_headers.data()), elf_header.section_num * sizeof(Elf32_SectionHeader));
    if (!file) {
        std::cerr << "Error reading section headers from file: " << filename << std::endl;
        return 1;
    }
    if (elf_header.section_table_index >= section_headers.size()) {
        std::cerr << "Invalid section header string table index." << std::endl;
        return 1;
    }

    const Elf32_SectionHeader& shstrtab = section_headers[elf_header.section_table_index];
    std::vector<char> shstrtab_data(shstrtab.sh_size);
    file.seekg(shstrtab.sh_offset);
    file.read(shstrtab_data.data(), shstrtab.sh_size);
    if (!file) {
        std::cerr << "Error reading section header string table from file: " << filename << std::endl;
        return 1;
    }

    std::cout << "Section Headers:" << std::endl;
    for (size_t i = 0; i < section_headers.size(); ++i)
    {
        const Elf32_SectionHeader& sh = section_headers[i];
        const char* name = shstrtab_data.data() + sh.sh_name;

        std::cout << "  Section " << i << ": " << name << std::endl;
        std::cout << "    Type: " << sh.sh_type << std::endl;
        std::cout << "    Flags: " << sh.sh_flags << std::endl;
        std::cout << "    Address: " << std::hex << sh.sh_addr << std::dec << std::endl;
        std::cout << "    Offset: " << sh.sh_offset << std::endl;
        std::cout << "    Size: " << sh.sh_size << std::endl;
        std::cout << "    Link: " << sh.sh_link << std::endl;
        std::cout << "    Info: " << sh.sh_info << std::endl;
        std::cout << "    Address alignment: " << sh.sh_addralign << std::endl;
        std::cout << "    Entry size: " << sh.sh_entsize << std::endl;
    }
    file.close();
    return 0;
}