#include "HuffmanTree.h"
#include <queue>
#include <vector>
#include <iostream>
#include <iomanip>
#include <stdexcept>

void HuffmanTree::build(const FrequencyMap& freqMap) {
    if (freqMap.empty()) {
        throw std::invalid_argument("HuffmanTree::build: frequency map is empty.");
    }

    root_ = nullptr;
    codeMap_.clear();
    singleSymbol_ = false;

    // ---- Degenerate case: exactly one unique character ----
    // There's no tree to traverse for codes; assign a fixed 1-bit code "0".
    // Decoding for this case is handled separately in decode() using
    // symbolCount directly, so the "0" code is really only used by
    // encode() to produce a non-empty bitstring (so 0-length input also
    // round-trips fine when symbolCount == 0).
    if (freqMap.size() == 1) {
        const auto& [ch, freq] = *freqMap.begin();
        root_ = std::make_shared<Node>(ch, freq);
        codeMap_[ch] = "0";
        singleSymbol_ = true;
        singleSymbolChar_ = ch;
        return;
    }

    // ---- General case: 2+ unique characters ----
    std::priority_queue<NodePtr, std::vector<NodePtr>, NodeComparator> minHeap;
    for (const auto& [ch, freq] : freqMap) {
        minHeap.push(std::make_shared<Node>(ch, freq));
    }

    while (minHeap.size() > 1) {
        NodePtr left = minHeap.top();  minHeap.pop();
        NodePtr right = minHeap.top(); minHeap.pop();
        const uint64_t combined = left->frequency + right->frequency;
        minHeap.push(std::make_shared<Node>(combined, std::move(left), std::move(right)));
    }

    root_ = minHeap.top();

    std::string path;
    path.reserve(64);
    generateCodes(root_, path);
}

void HuffmanTree::generateCodes(const NodePtr& node, std::string& path) {
    if (!node) return;

    if (node->isLeaf()) {
        // path is guaranteed non-empty here because build() only calls this
        // when there are 2+ unique characters, so root_ is always internal.
        codeMap_[node->character] = path;
        return;
    }

    path.push_back('0');
    generateCodes(node->left, path);
    path.pop_back();

    path.push_back('1');
    generateCodes(node->right, path);
    path.pop_back();
}

std::string HuffmanTree::encode(const std::string& input) const {
    std::string bitString;
    bitString.reserve(input.size() * 2); // rough upper-bound guess

    for (const unsigned char ch : input) {
        const auto it = codeMap_.find(static_cast<uint8_t>(ch));
        if (it == codeMap_.end()) {
            throw std::runtime_error(
                "HuffmanTree::encode: byte value " + std::to_string(static_cast<int>(ch)) +
                " has no assigned code (not present in the frequency table used to build the tree).");
        }
        bitString += it->second;
    }
    return bitString;
}

std::string HuffmanTree::decode(const std::string& bitString, uint64_t symbolCount) const {
    if (!root_) {
        throw std::runtime_error("HuffmanTree::decode: tree has not been built.");
    }

    std::string output;
    output.reserve(symbolCount);

    // Degenerate single-symbol case: every original byte is the same
    // character. We don't even need to look at bitString — symbolCount
    // alone tells us exactly how many copies to emit. This sidesteps any
    // ambiguity about how many bits represent each occurrence.
    if (singleSymbol_) {
        output.assign(static_cast<size_t>(symbolCount), static_cast<char>(singleSymbolChar_));
        return output;
    }

    NodePtr current = root_;
    size_t bitIndex = 0;
    const size_t totalBits = bitString.size();

    while (output.size() < symbolCount) {
        if (bitIndex >= totalBits) {
            throw std::runtime_error(
                "HuffmanTree::decode: bitstream exhausted before all symbols were decoded "
                "(expected " + std::to_string(symbolCount) + ", got " + std::to_string(output.size()) + ").");
        }

        const char bit = bitString[bitIndex++];
        if (bit == '0') {
            current = current->left;
        } else if (bit == '1') {
            current = current->right;
        } else {
            throw std::runtime_error("HuffmanTree::decode: invalid character in bitstream (expected '0' or '1').");
        }

        if (!current) {
            throw std::runtime_error("HuffmanTree::decode: invalid bit sequence — fell off the tree.");
        }

        if (current->isLeaf()) {
            output.push_back(static_cast<char>(current->character));
            current = root_;
        }
    }

    return output;
}

void HuffmanTree::printCodes() const {
    std::cout << "\n";
    std::cout << std::left << std::setw(14) << "Character"
              << std::setw(8) << "ASCII" << "Code\n";
    std::cout << std::string(48, '-') << "\n";

    for (const auto& [ch, code] : codeMap_) {
        std::string display;
        switch (ch) {
            case '\n': display = "\\n";    break;
            case '\t': display = "\\t";    break;
            case '\r': display = "\\r";    break;
            case ' ':  display = "SPACE";  break;
            default:
                if (ch < 32 || ch == 127) {
                    display = "0x" + std::to_string(static_cast<int>(ch));
                } else {
                    display = std::string(1, static_cast<char>(ch));
                }
        }
        std::cout << std::left << std::setw(14) << display
                  << std::setw(8) << static_cast<int>(ch)
                  << code << "\n";
    }
    std::cout << "\n";
}
