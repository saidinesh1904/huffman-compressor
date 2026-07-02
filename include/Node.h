#ifndef NODE_H
#define NODE_H

#include <memory>
#include <cstdint>

struct Node;
using NodePtr = std::shared_ptr<Node>;

/**
 * Node — a single node in the Huffman binary tree.
 *
 * Leaf nodes represent exactly one byte value (0-255); left/right are null.
 * Internal nodes aggregate the frequency of their two children and do not
 * represent any character themselves.
 *
 * The tree is always a full binary tree: every internal node has exactly
 * two children. This invariant is maintained by HuffmanTree::build().
 */
struct Node {
    uint8_t  character;  ///< Byte value this leaf represents (meaningless for internal nodes)
    uint64_t frequency;  ///< Frequency count (leaf) or combined frequency (internal)
    bool     leaf;        ///< Explicit leaf flag — do NOT infer from left/right being null
    uint64_t minLeafChar;  ///< Smallest character value in this node's subtree. Used as a
                            ///< deterministic, content-based tie-breaker (NOT insertion order,
                            ///< which can legitimately differ between two unordered_maps built
                            ///< from the same data — e.g. compress-time map vs the map rebuilt
                            ///< from the .huff file's frequency table during decompress).
    NodePtr  left;
    NodePtr  right;

    /// Leaf constructor.
    Node(uint8_t ch, uint64_t freq);

    /// Internal node constructor — frequency is the sum of both children.
    Node(uint64_t freq, NodePtr l, NodePtr r);

    bool isLeaf() const noexcept { return leaf; }
};

/**
 * Min-heap comparator for std::priority_queue.
 *
 * std::priority_queue is a max-heap by default; returning (a > b) inverts
 * the ordering so the smallest-frequency node is always at top().
 * Ties are broken by character value to make tree construction fully
 * deterministic regardless of unordered_map iteration order.
 */
struct NodeComparator {
    bool operator()(const NodePtr& a, const NodePtr& b) const noexcept;
};

#endif // NODE_H
