#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include "HuffmanTree.h"
#include "FileManager.h"
#include <string>

/**
 * Decompressor — reads a .huff file, reconstructs the identical Huffman
 * tree from its embedded frequency table (build() is deterministic given
 * the same frequencies, so this always matches the tree used to compress),
 * decodes, and writes the result.
 */
class Decompressor {
public:
    Decompressor() = default;

    void decompress(const std::string& inputPath, const std::string& outputPath);

    const std::string& getDecodedContent() const noexcept { return decodedContent_; }

private:
    FileManager fileManager_;
    std::string decodedContent_;
};

#endif // DECOMPRESSOR_H
