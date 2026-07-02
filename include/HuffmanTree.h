#ifndef HUFFMANTREE_H
#define HUFFMANTREE_H

#include "Node.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <cstdint>

/**
 * HuffmanTree — builds the tree from a frequency table, generates
 * prefix-free binary codes, and encodes/decodes data against that tree.
 *
 * Special case: if the input has exactly ONE unique byte value, there is
 * no meaningful binary tree (a single leaf has no left/right edges to
 * derive a code from). This class handles that case explicitly by
 * assigning the single character the 1-bit code "0", and storing the
 * symbol count explicitly during decode (see decode()'s singleSymbol path).
 */
class HuffmanTree {
public:
    using FrequencyMap = std::unordered_map<uint8_t, uint64_t>;
    using CodeMap       = std::unordered_map<uint8_t, std::string>;

    HuffmanTree() = default;

    /**
     * Build the Huffman tree and code table from a frequency map.
     * @throws std::invalid_argument if freqMap is empty.
     */
    void build(const FrequencyMap& freqMap);

    /**
     * Encode raw bytes into a string of '0'/'1' characters using the
     * code table built by build().
     * @throws std::runtime_error if a byte has no assigned code (tree
     *         not built, or byte absent from the original frequency map).
     */
    std::string encode(const std::string& input) const;

    /**
     * Decode a bitstring (as produced by encode()) back into the original
     * bytes. Exactly `symbolCount` symbols are emitted, which is the
     * authoritative stopping condition (NOT the number of bits consumed).
     * This guarantees decode never overruns or underruns the original
     * data, regardless of padding bits at the end of the bitstring.
     *
     * @param bitString    The full bit sequence (no padding stripped needed,
     *                     decode stops based on symbolCount).
     * @param symbolCount  Exact number of original symbols to reconstruct.
     * @throws std::runtime_error on malformed/truncated bitstream.
     */
    std::string decode(const std::string& bitString, uint64_t symbolCount) const;

    const CodeMap& getCodeMap() const noexcept { return codeMap_; }
    NodePtr getRoot() const noexcept { return root_; }

    /// True if the tree was built from a frequency map with exactly one
    /// unique character (degenerate single-leaf case).
    bool isSingleSymbol() const noexcept { return singleSymbol_; }

    /// The lone character, valid only when isSingleSymbol() is true.
    uint8_t getSingleSymbolChar() const noexcept { return singleSymbolChar_; }

    void printCodes() const;

private:
    NodePtr root_;
    CodeMap codeMap_;
    bool    singleSymbol_ = false;
    uint8_t singleSymbolChar_ = 0;

    void generateCodes(const NodePtr& node, std::string& code);
};

#endif // HUFFMANTREE_H
