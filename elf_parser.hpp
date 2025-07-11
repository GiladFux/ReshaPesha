// include/elf_parser.hpp
#ifndef ELF_PARSER_HPP
#define ELF_PARSER_HPP

#include <string>
#include <vector>
#include <cstdint>

struct Elf32Header {
    unsigned char e_ident[16];
    uint16_t type;
    uint16_t arch;
    uint32_t version;
    uint32_t entry;
    uint32_t e_phoff;
    uint32_t section_offset;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t section_size;
    uint16_t section_num;
    uint16_t section_table_index;
};

struct Elf32SectionHeader {
    uint32_t sh_name;
    uint32_t sh_type;
    uint32_t sh_flags;
    uint32_t sh_addr;
    uint32_t sh_offset;
    uint32_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint32_t sh_addralign;
    uint32_t sh_entsize;
};

class Elf32Parser {
public:
    explicit Elf32Parser(const std::string& filepath);
    bool parse();
    const Elf32Header& getHeader() const;
    const std::vector<Elf32SectionHeader>& getSectionHeaders() const;
    std::vector<uint8_t> getSectionData(const std::string& section_name) const;
    std::string getError() const;

private:
    std::string filepath_;
    Elf32Header header_;
    std::vector<Elf32SectionHeader> section_headers_;
    std::vector<char> shstrtab_data_;
    std::string error_;
};

#endif