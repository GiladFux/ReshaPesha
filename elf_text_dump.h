#ifndef ELF_TEXT_DUMP_H
#define ELF_TEXT_DUMP_H

#include <string>

struct Elf32_Header;
struct Elf32_SectionHeader;

// Dumps ELF header and section info from a given ELF file path
// Returns true on success, false on failure
bool dump_elf_32(const std::string& filepath);

#endif // ELF_TEXT_DUMP_H
