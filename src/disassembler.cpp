#include "disassembler.hpp"
#include <sstream>

std::string reg32_name(unsigned char reg) {
    static const char* names[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
    return reg < 8 ? names[reg] : "unk";
}

std::vector<Instruction> disassemble_x86(const std::vector<unsigned char>& code, uint32_t base_addr) {
    std::vector<Instruction> result;
    size_t i = 0;
    while (i < code.size()) {
        unsigned char op = code[i];
        Instruction instr;
        instr.address = base_addr + i;
        instr.bytes.push_back(op);

        switch (op) {
            case 0x90: // NOP
                instr.mnemonic = "nop";
                instr.op_str = "";
                ++i;
                break;
            case 0xC3: // RET
                instr.mnemonic = "ret";
                instr.op_str = "";
                ++i;
                break;
            case 0xCC: // INT3
                instr.mnemonic = "int3";
                instr.op_str = "";
                ++i;
                break;
            case 0xCD: // INT imm8
                if (i + 1 < code.size()) {
                    instr.bytes.push_back(code[i + 1]);
                    instr.mnemonic = "int";
                    std::ostringstream oss;
                    oss << "0x" << std::hex << static_cast<int>(code[i + 1]);
                    instr.op_str = oss.str();
                    i += 2;
                } else {
                    instr.mnemonic = "db";
                    instr.op_str = "0xcd";
                    ++i;
                }
                break;
            case 0xE8: // CALL rel32
                if (i + 4 < code.size()) {
                    instr.bytes.insert(instr.bytes.end(), code.begin() + i + 1, code.begin() + i + 5);
                    int32_t rel = *reinterpret_cast<const int32_t*>(&code[i + 1]);
                    std::ostringstream oss;
                    oss << "0x" << std::hex << instr.address + 5 + rel;
                    instr.mnemonic = "call";
                    instr.op_str = oss.str();
                    i += 5;
                } else {
                    instr.mnemonic = "db";
                    instr.op_str = "0xe8";
                    ++i;
                }
                break;
            case 0xE9: // JMP rel32
                if (i + 4 < code.size()) {
                    instr.bytes.insert(instr.bytes.end(), code.begin() + i + 1, code.begin() + i + 5);
                    int32_t rel = *reinterpret_cast<const int32_t*>(&code[i + 1]);
                    std::ostringstream oss;
                    oss << "0x" << std::hex << instr.address + 5 + rel;
                    instr.mnemonic = "jmp";
                    instr.op_str = oss.str();
                    i += 5;
                } else {
                    instr.mnemonic = "db";
                    instr.op_str = "0xe9";
                    ++i;
                }
                break;
            case 0xEB: // JMP rel8
                if (i + 1 < code.size()) {
                    instr.bytes.push_back(code[i + 1]);
                    int8_t rel = static_cast<int8_t>(code[i + 1]);
                    std::ostringstream oss;
                    oss << "0x" << std::hex << instr.address + 2 + rel;
                    instr.mnemonic = "jmp";
                    instr.op_str = oss.str();
                    i += 2;
                } else {
                    instr.mnemonic = "db";
                    instr.op_str = "0xeb";
                    ++i;
                }
                break;
            // ADD reg32, reg32: 0x01 /r
            case 0x01:
                if (i + 1 < code.size()) {
                    instr.bytes.push_back(code[i + 1]);
                    uint8_t modrm = code[i + 1];
                    uint8_t reg = (modrm >> 3) & 0x7;
                    uint8_t rm  = modrm & 0x7;
                    instr.mnemonic = "add";
                    instr.op_str = reg32_name(rm) + ", " + reg32_name(reg);
                    i += 2;
                } else {
                    instr.mnemonic = "db";
                    instr.op_str = "0x01";
                    ++i;
                }
                break;

            // XOR reg32, reg32: 0x31 /r
            case 0x31:
                if (i + 1 < code.size()) {
                    instr.bytes.push_back(code[i + 1]);
                    uint8_t modrm = code[i + 1];
                    uint8_t reg = (modrm >> 3) & 0x7;
                    uint8_t rm  = modrm & 0x7;
                    instr.mnemonic = "xor";
                    instr.op_str = reg32_name(rm) + ", " + reg32_name(reg);
                    i += 2;
                } else {
                    instr.mnemonic = "db";
                    instr.op_str = "0x31";
                    ++i;
                }
                break;

            // SUB reg32, reg32: 0x29 /r
            case 0x29:
                if (i + 1 < code.size()) {
                    instr.bytes.push_back(code[i + 1]);
                    uint8_t modrm = code[i + 1];
                    uint8_t reg = (modrm >> 3) & 0x7;
                    uint8_t rm  = modrm & 0x7;
                    instr.mnemonic = "sub";
                    instr.op_str = reg32_name(rm) + ", " + reg32_name(reg);
                    i += 2;
                } else {
                    instr.mnemonic = "db";
                    instr.op_str = "0x29";
                    ++i;
                }
                break;

            // MOV reg32, reg32: 0x89 /r
            case 0x89:
                if (i + 1 < code.size()) {
                    instr.bytes.push_back(code[i + 1]);
                    uint8_t modrm = code[i + 1];
                    uint8_t reg = (modrm >> 3) & 0x7;
                    uint8_t rm  = modrm & 0x7;
                    instr.mnemonic = "mov";
                    instr.op_str = reg32_name(rm) + ", " + reg32_name(reg);
                    i += 2;
                } else {
                    instr.mnemonic = "db";
                    instr.op_str = "0x89";
                    ++i;
                }
                break;

            // MOV reg32, reg32: 0x8B /r
            case 0x8B:
                if (i + 1 < code.size()) {
                    instr.bytes.push_back(code[i + 1]);
                    uint8_t modrm = code[i + 1];
                    uint8_t reg = (modrm >> 3) & 0x7;
                    uint8_t rm  = modrm & 0x7;
                    instr.mnemonic = "mov";
                    instr.op_str = reg32_name(reg) + ", " + reg32_name(rm);
                    i += 2;
                } else {
                    instr.mnemonic = "db";
                    instr.op_str = "0x8B";
                    ++i;
                }
                break;

            // PUSH imm8: 0x6A
            case 0x6A:
                if (i + 1 < code.size()) {
                    instr.bytes.push_back(code[i + 1]);
                    instr.mnemonic = "push";
                    std::ostringstream oss;
                    oss << "0x" << std::hex << static_cast<int>(code[i + 1]);
                    instr.op_str = oss.str();
                    i += 2;
                } else {
                    instr.mnemonic = "db";
                    instr.op_str = "0x6A";
                    ++i;
                }
                break;

            default:
                // POP reg: 0x58–0x5F
                if (op >= 0x58 && op <= 0x5F) {
                    instr.mnemonic = "pop";
                    instr.op_str = reg32_name(op - 0x58);
                    ++i;
                }
                // INC reg32: 0x40–0x47
                else if (op >= 0x40 && op <= 0x47) {
                    instr.mnemonic = "inc";
                    instr.op_str = reg32_name(op - 0x40);
                    ++i;
                }
                // DEC reg32: 0x48–0x4F
                else if (op >= 0x48 && op <= 0x4F) {
                    instr.mnemonic = "dec";
                    instr.op_str = reg32_name(op - 0x48);
                    ++i;
                }
                // MOV reg, imm32: 0xB8–0xBF
                else if (op >= 0xB8 && op <= 0xBF && i + 4 < code.size()) {
                    instr.bytes.insert(instr.bytes.end(), code.begin() + i + 1, code.begin() + i + 5);
                    instr.mnemonic = "mov";
                    std::ostringstream oss;
                    oss << reg32_name(op - 0xB8) << ", 0x"
                        << std::hex << *reinterpret_cast<const uint32_t*>(&code[i + 1]);
                    instr.op_str = oss.str();
                    i += 5;
                }
                // PUSH reg: 0x50–0x57
                else if (op >= 0x50 && op <= 0x57) {
                    instr.mnemonic = "push";
                    instr.op_str = reg32_name(op - 0x50);
                    ++i;
                }
                // Unknown opcode, just print as DB
                else {
                    std::ostringstream oss;
                    oss << "0x" << std::hex << static_cast<int>(op);
                    instr.mnemonic = "db";
                    instr.op_str = oss.str();
                    ++i;
                }
                break;
        }
        result.push_back(instr);
    }
    return result;
}