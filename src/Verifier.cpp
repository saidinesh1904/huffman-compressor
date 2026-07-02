#include "Verifier.h"
#include <iostream>
#include <iomanip>
#include <cstdint>

bool Verifier::verify(const std::string& original, const std::string& decoded) const {
    if (original.size() != decoded.size()) {
        std::cout << "[Verifier] Size mismatch: original=" << original.size()
                  << " bytes, decoded=" << decoded.size() << " bytes.\n";
        return false;
    }

    for (size_t i = 0; i < original.size(); ++i) {
        if (original[i] != decoded[i]) {
            std::cout << "[Verifier] Mismatch at byte " << i
                      << ": expected 0x" << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(static_cast<uint8_t>(original[i]))
                      << ", got 0x" << std::setw(2) << std::setfill('0')
                      << static_cast<int>(static_cast<uint8_t>(decoded[i]))
                      << std::dec << std::setfill(' ') << "\n";
            return false;
        }
    }
    return true;
}

void Verifier::printResult(bool success) const {
    std::cout << "\n========================================\n";
    if (success) {
        std::cout << "  Verification : SUCCESS\n";
        std::cout << "  Original == Decoded\n";
    } else {
        std::cout << "  Verification : FAILED\n";
        std::cout << "  Files do not match!\n";
    }
    std::cout << "========================================\n\n";
}
