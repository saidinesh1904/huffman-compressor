#include "Node.h"
#include <algorithm>
#include <cstdint>

Node::Node(uint8_t ch, uint64_t freq)
    : character(ch)
    , frequency(freq)
    , leaf(true)
    , minLeafChar(ch)
    , left(nullptr)
    , right(nullptr)
{
}

Node::Node(uint64_t freq, NodePtr l, NodePtr r)
    : character(0)
    , frequency(freq)
    , leaf(false)
    , minLeafChar(std::min(l ? l->minLeafChar : UINT64_MAX,
                           r ? r->minLeafChar : UINT64_MAX))
    , left(std::move(l))
    , right(std::move(r))
{
}

bool NodeComparator::operator()(const NodePtr& a, const NodePtr& b) const noexcept {
    if (a->frequency != b->frequency) {
        return a->frequency > b->frequency;
    }
    // Strict weak ordering on ties, based purely on content (the smallest
    // character value reachable in each node's subtree). This is invariant
    // to unordered_map iteration order, so the SAME tree shape is produced
    // whether the frequency map was built from scanning the original file
    // or reconstructed from the .huff header during decompression — which
    // is essential, since decode() only works if both trees are identical.
    return a->minLeafChar > b->minLeafChar;
}


