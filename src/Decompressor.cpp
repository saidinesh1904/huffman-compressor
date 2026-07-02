#include "Decompressor.h"
#include <stdexcept>
#include <iostream>

void Decompressor::decompress(const std::string& inputPath, const std::string& outputPath) {
    if (!fileManager_.fileExists(inputPath)) {
        throw std::runtime_error("Decompressor::decompress: compressed file not found: '" + inputPath + "'.");
    }

    HuffmanTree::FrequencyMap freqMap;
    std::string bitString;
    uint64_t symbolCount = 0;

    fileManager_.readCompressedFile(inputPath, freqMap, bitString, symbolCount);

    // Empty-file case: no frequency table at all.
    if (freqMap.empty()) {
        if (symbolCount != 0) {
            throw std::runtime_error("Decompressor::decompress: corrupt .huff file (empty table but non-zero symbol count).");
        }
        decodedContent_.clear();
        fileManager_.writeTextFile(outputPath, decodedContent_);
        std::cout << "[Decompressor] Decompressed empty file '" << inputPath << "' -> '" << outputPath << "'\n";
        return;
    }

    HuffmanTree huffmanTree;
    huffmanTree.build(freqMap);

    decodedContent_ = huffmanTree.decode(bitString, symbolCount);
    fileManager_.writeTextFile(outputPath, decodedContent_);

    std::cout << "[Decompressor] Decompressed '" << inputPath << "' -> '" << outputPath << "'\n";
}
