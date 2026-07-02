#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "HuffmanTree.h"
#include "FileManager.h"
#include <string>
#include <unordered_map>
#include <cstdint>

/**
 * Compressor — orchestrates: read -> frequency count -> build tree
 * -> encode -> write .huff.
 *
 * Empty-file policy: compressing a zero-byte file produces a valid .huff
 * file with an empty (but non-trivial — see note in compress()) state;
 * decompressing it reproduces a zero-byte file. This is treated as a
 * supported edge case, not an error.
 */
class Compressor {
public:
    using FrequencyMap = std::unordered_map<uint8_t, uint64_t>;

    Compressor() = default;

    void compress(const std::string& inputPath, const std::string& outputPath);

    const HuffmanTree& getHuffmanTree() const noexcept { return huffmanTree_; }
    const FrequencyMap& getFrequencyMap() const noexcept { return freqMap_; }
    const std::string& getOriginalContent() const noexcept { return originalContent_; }

private:
    HuffmanTree huffmanTree_;
    FrequencyMap freqMap_;
    FileManager fileManager_;
    std::string originalContent_;

    static FrequencyMap buildFrequencyMap(const std::string& content);
};

#endif // COMPRESSOR_H
