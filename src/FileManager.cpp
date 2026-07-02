#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

namespace fs = std::filesystem;

std::string FileManager::readTextFile(const std::string& path) const {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("FileManager::readTextFile: cannot open '" + path + "'.");
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    if (file.bad()) {
        throw std::runtime_error("FileManager::readTextFile: I/O error reading '" + path + "'.");
    }
    return ss.str();
}

void FileManager::writeTextFile(const std::string& path, const std::string& content) const {
    std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        throw std::runtime_error("FileManager::writeTextFile: cannot open '" + path + "' for writing.");
    }
    if (!content.empty()) {
        file.write(content.data(), static_cast<std::streamsize>(content.size()));
    }
    if (file.fail()) {
        throw std::runtime_error("FileManager::writeTextFile: I/O error writing '" + path + "'.");
    }
}

void FileManager::writeCompressedFile(const std::string& path,
                                      const FrequencyMap& freqMap,
                                      const std::string& bitString,
                                      uint64_t symbolCount) const {
    std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        throw std::runtime_error("FileManager::writeCompressedFile: cannot open '" + path + "' for writing.");
    }

    const auto writeU64 = [&file](uint64_t v) {
        file.write(reinterpret_cast<const char*>(&v), sizeof(v));
    };

    writeU64(kMagic);

    const uint64_t numEntries = static_cast<uint64_t>(freqMap.size());
    writeU64(numEntries);

    for (const auto& [ch, freq] : freqMap) {
        file.put(static_cast<char>(ch));
        writeU64(freq);
    }

    writeU64(symbolCount);

    const uint64_t bitCount = static_cast<uint64_t>(bitString.size());
    writeU64(bitCount);

    // Pack bits MSB-first into bytes, zero-padding the final byte.
    uint8_t currentByte = 0;
    int bitsInByte = 0;
    for (const char bit : bitString) {
        currentByte = static_cast<uint8_t>((currentByte << 1) | (bit == '1' ? 1 : 0));
        if (++bitsInByte == 8) {
            file.put(static_cast<char>(currentByte));
            currentByte = 0;
            bitsInByte = 0;
        }
    }
    if (bitsInByte > 0) {
        currentByte = static_cast<uint8_t>(currentByte << (8 - bitsInByte));
        file.put(static_cast<char>(currentByte));
    }

    if (file.fail()) {
        throw std::runtime_error("FileManager::writeCompressedFile: I/O error writing '" + path + "'.");
    }
}

void FileManager::readCompressedFile(const std::string& path,
                                     FrequencyMap& freqMap,
                                     std::string& bitString,
                                     uint64_t& symbolCount) const {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("FileManager::readCompressedFile: cannot open '" + path + "'.");
    }

    const auto readU64 = [&file](const char* what) -> uint64_t {
        uint64_t v = 0;
        file.read(reinterpret_cast<char*>(&v), sizeof(v));
        if (file.fail()) {
            throw std::runtime_error(std::string("FileManager::readCompressedFile: truncated/invalid header (") + what + ").");
        }
        return v;
    };

    const uint64_t magic = readU64("magic");
    if (magic != kMagic) {
        throw std::runtime_error("FileManager::readCompressedFile: not a valid .huff file (bad magic).");
    }

    const uint64_t numEntries = readU64("numEntries");
    // Sanity bound: at most 256 unique byte values can ever exist.
    // numEntries == 0 is permitted — it represents an originally empty file.
    if (numEntries > 256) {
        throw std::runtime_error("FileManager::readCompressedFile: corrupt frequency table size.");
    }

    freqMap.clear();
    freqMap.reserve(static_cast<size_t>(numEntries));
    for (uint64_t i = 0; i < numEntries; ++i) {
        char chRaw = 0;
        file.get(chRaw);
        if (file.fail()) {
            throw std::runtime_error("FileManager::readCompressedFile: truncated frequency table.");
        }
        const uint64_t freq = readU64("frequency entry");
        freqMap[static_cast<uint8_t>(chRaw)] = freq;
    }

    symbolCount = readU64("symbolCount");
    const uint64_t bitCount = readU64("bitCount");

    const uint64_t fullBytes = bitCount / 8;
    const uint64_t remainderBits = bitCount % 8;
    const uint64_t bytesToRead = fullBytes + (remainderBits > 0 ? 1 : 0);

    bitString.clear();
    bitString.reserve(static_cast<size_t>(bitCount));

    for (uint64_t i = 0; i < bytesToRead; ++i) {
        char byteRaw = 0;
        file.get(byteRaw);
        if (file.fail()) {
            throw std::runtime_error("FileManager::readCompressedFile: truncated compressed data.");
        }
        const uint8_t byte = static_cast<uint8_t>(byteRaw);

        // Determine how many bits from this byte are meaningful (last byte
        // may be partially padded).
        const int bitsFromThisByte =
            (i == bytesToRead - 1 && remainderBits > 0) ? static_cast<int>(remainderBits) : 8;

        for (int b = 7; b >= 8 - bitsFromThisByte; --b) {
            bitString.push_back(((byte >> b) & 1) ? '1' : '0');
        }
    }

    if (bitString.size() != bitCount) {
        throw std::runtime_error("FileManager::readCompressedFile: bit count mismatch after unpacking.");
    }
}

uint64_t FileManager::getFileSize(const std::string& path) const {
    std::error_code ec;
    const auto size = fs::file_size(path, ec);
    return ec ? 0 : static_cast<uint64_t>(size);
}

bool FileManager::fileExists(const std::string& path) const {
    std::error_code ec;
    return fs::exists(path, ec) && !ec && fs::is_regular_file(path, ec);
}
