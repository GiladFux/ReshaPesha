// src/elf_parser.cpp
#include "elf_parser.hpp"
#include <fstream>
#include <iostream>
#include <cstring>

Elf32Parser::Elf32Parser(const std::string& filepath) : filepath_(filepath) {}

bool Elf32Parser::parse() 
{
    std::ifstream file(filepath_, std::ios::binary);
    if (!file) 
    {
        error_ = "Error opening file: " + filepath_;
        return false;
    }

    file.read(reinterpret_cast<char*>(&header_), sizeof(Elf32Header)); //read expects char* so we elf32Header into char* 
    if (!file) 
    {
        error_ = "Error reading ELF header from file: " + filepath_;
        file.close();
        return false;
    }

    if (std::memcmp(header_.e_ident, "\x7f""ELF", 4) != 0) //compare the first 4 bytes of e_ident with the ELF magic number
    {
        error_ = "Not a valid ELF file: " + filepath_;
        file.close();
        return false;
    }

    file.seekg(header_.section_offset); //move pointer to the section headers
    section_headers_.resize(header_.section_num); //resize the vector to hold the section headers
    file.read(reinterpret_cast<char*>(section_headers_.data()), header_.section_num * sizeof(Elf32SectionHeader)); //read the section headers into the vector
    if (!file) 
    {
        error_ = "Error reading section headers from file: " + filepath_;
        file.close();
        return false;
    }

    if (header_.section_table_index >= section_headers_.size()) 
    {
        error_ = "Invalid section header string table index";
        file.close();
        return false;
    }

    const Elf32SectionHeader& shstrtab = section_headers_[header_.section_table_index]; 
    shstrtab_data_.resize(shstrtab.sh_size);
    file.seekg(shstrtab.sh_offset);
    file.read(shstrtab_data_.data(), shstrtab.sh_size);
    if (!file) 
    {
        error_ = "Error reading section header string table from file: " + filepath_;
        file.close();
        return false;
    }

    file.close();
    return true;
}

const Elf32Header& Elf32Parser::getHeader() const 
{
    return header_;
}

const std::vector<Elf32SectionHeader>& Elf32Parser::getSectionHeaders() const 
{
    return section_headers_;
}

std::vector<uint8_t> Elf32Parser::getSectionData(const std::string& section_name) const 
{
    std::ifstream file(filepath_, std::ios::binary);
    if (!file) {
        return {};
    }

    for (const auto& sh : section_headers_) {
        const char* name = shstrtab_data_.data() + sh.sh_name;
        if (std::strcmp(name, section_name.c_str()) == 0) {
            std::vector<uint8_t> data(sh.sh_size);
            file.seekg(sh.sh_offset);
            file.read(reinterpret_cast<char*>(data.data()), sh.sh_size);
            file.close();
            return data;
        }
    }
    file.close();
    return {};
}

uint32_t Elf32Parser::getSectionAddr(const std::string& section_name) const {
    for (const auto& sh : section_headers_) {
        const char* name = shstrtab_data_.data() + sh.sh_name;
        if (std::strcmp(name, section_name.c_str()) == 0) {
            return sh.sh_addr;
        }
    }
    return 0; // Not found
}

uint32_t Elf32Parser::getSymbolAddr(const std::string& symbol_name) const {
    // Find .symtab and .strtab sections
    const Elf32SectionHeader* symtab = nullptr;
    const Elf32SectionHeader* strtab = nullptr;
    for (const auto& sh : section_headers_) {
        const char* name = shstrtab_data_.data() + sh.sh_name;
        if (std::strcmp(name, ".symtab") == 0) symtab = &sh;
        if (std::strcmp(name, ".strtab") == 0) strtab = &sh;
    }
    if (!symtab || !strtab) return 0;

    // Read symbol table
    std::ifstream file(filepath_, std::ios::binary);
    if (!file) return 0;

    file.seekg(symtab->sh_offset);
    size_t num_symbols = symtab->sh_size / symtab->sh_entsize;
    struct Elf32Symbol {
        uint32_t st_name;
        uint32_t st_value;
        uint32_t st_size;
        uint8_t  st_info;
        uint8_t  st_other;
        uint16_t st_shndx;
    };
    std::vector<Elf32Symbol> symbols(num_symbols);
    file.read(reinterpret_cast<char*>(symbols.data()), symtab->sh_size);

    // Read string table
    std::vector<char> strtab_data(strtab->sh_size);
    file.seekg(strtab->sh_offset);
    file.read(strtab_data.data(), strtab->sh_size);

    // Search for symbol
    for (const auto& sym : symbols) {
        const char* name = strtab_data.data() + sym.st_name;
        if (symbol_name == name) {
            return sym.st_value;
        }
    }
    return 0;
}

std::string Elf32Parser::getError() const {
    return error_;
}