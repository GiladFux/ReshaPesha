#include "instruction.hpp"
#include <iomanip>
#include <sstream>

std::string Instruction::toString() const {
    std::stringstream ss;
    // Format address (e.g., "0x8048000")
    ss << std::hex << std::setw(8) << std::setfill('0') << address << ":  ";
    
    // Format raw bytes (e.g., "89 c3 ")
    for (const auto& b : bytes) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b) << " ";
    }
    
    // Pad bytes to align mnemonic (adjust padding based on max instruction length, e.g., 15 bytes)
    ss << std::string(std::max(0, 15 * 3 - static_cast<int>(bytes.size()) * 3), ' ');
    
    // Format mnemonic and operands (e.g., "mov eax, ebx")
    ss << mnemonic;
    if (!op_str.empty()) {
        ss << " " << op_str;
    }
    
    return ss.str();
}
