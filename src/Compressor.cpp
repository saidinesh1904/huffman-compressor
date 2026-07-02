#include "Compressor.h"
#include <stdexcept>
#include <iostream>

Compressor::FrequencyMap Compressor::buildFrequencyMap(const std::string& content) {
    FrequencyMap freqMap;
    freqMap.reserve(256);
    for (const unsigned char ch : content) {
        ++freqMap[static_cast<uint8_t>(ch)];
    }
    return freqMap;
}

void Compressor::compress(const std::string& inputPath, const std::string& outputPath) {
    if (!fileManager_.fileExists(inputPath)) {
        throw std::runtime_error("Compressor::compress: input file not found: '" + inputPath + "'.");
    }

    originalContent_ = fileManager_.readTextFile(inputPath);
    freqMap_ = buildFrequencyMap(originalContent_);

    // Empty file: no tree to build, write an empty-table .huff file directly.
    if (freqMap_.empty()) {
        fileManager_.writeCompressedFile(outputPath, freqMap_, /*bitString=*/"", /*symbolCount=*/0);
        std::cout << "[Compressor] Compressed empty file '" << inputPath << "' -> '" << outputPath << "'\n";
        return;
    }

    huffmanTree_.build(freqMap_);
    const std::string bitString = huffmanTree_.encode(originalContent_);
    const uint64_t symbolCount = static_cast<uint64_t>(originalContent_.size());

    fileManager_.writeCompressedFile(outputPath, freqMap_, bitString, symbolCount);
    std::cout << "[Compressor] Compressed '" << inputPath << "' -> '" << outputPath << "'\n";
}
