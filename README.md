# Huffman File Compression Utility

A production-quality command-line file compression tool implementing
Huffman Coding from scratch in modern C++17. Works correctly on text
and arbitrary binary input, including empty files and single-character
files.

## Features

- Lossless compression and decompression of any file (text or binary)
- Self-describing binary `.huff` format (embedded frequency table, magic
  number, exact symbol count for robust decode termination)
- Deterministic Huffman tree construction — the SAME tree is rebuilt
  during decompression from the embedded frequency table, independent of
  `unordered_map` iteration order
- Compression ratio, timing, and byte-savings statistics
- Automatic round-trip verification (original == decoded)
- Huffman code table display
- Explicit handling of: empty files, single-unique-character files,
  files with all-unique characters, equal-frequency ties, and full
  256-byte-value binary data
- Robust error handling: missing files, corrupt/truncated `.huff` files,
  invalid magic numbers
- Verified clean under `-Wall -Wextra -Wpedantic` and AddressSanitizer +
  UndefinedBehaviorSanitizer

## Folder Structure

```
Huffman-Compressor/
├── include/          # Header files
│   ├── Node.h
│   ├── HuffmanTree.h
│   ├── Compressor.h
│   ├── Decompressor.h
│   ├── FileManager.h
│   ├── Statistics.h
│   └── Verifier.h
├── src/              # Implementation files
│   ├── Node.cpp
│   ├── HuffmanTree.cpp
│   ├── Compressor.cpp
│   ├── Decompressor.cpp
│   ├── FileManager.cpp
│   ├── Statistics.cpp
│   └── Verifier.cpp
├── data/             # Sample input
│   └── sample.txt
├── output/           # Generated output files (created at build/run time)
├── main.cpp
├── Makefile
└── README.md
```

## How Huffman Coding Works

1. **Count frequencies** — scan the input, count how often each byte
   value appears.
2. **Build a min-heap** — one leaf node per unique byte, ordered by
   frequency.
3. **Build the tree** — repeatedly extract the two lowest-frequency
   nodes, merge into an internal node, reinsert. Repeat until one node
   (the root) remains. Ties are broken deterministically using the
   smallest character value reachable in each node's subtree — this
   guarantees the tree shape depends only on the (character, frequency)
   pairs, not on map iteration order, which is essential since the tree
   is rebuilt independently during decompression.
4. **Generate codes** — DFS from root: left edge = `0`, right edge =
   `1`. Each leaf's root-to-leaf path is its prefix-free binary code.
5. **Encode** — replace each byte with its code; pack bits MSB-first
   into bytes, zero-padding the final byte.
6. **Decode** — rebuild the identical tree from the saved frequency
   table; walk bit-by-bit, emitting a byte each time a leaf is reached,
   stopping after exactly `symbolCount` bytes (the original file size,
   stored in the header) — NOT when bits run out. This is what makes
   decoding immune to trailing zero-padding bits being misinterpreted
   as additional valid symbols.

### Special cases
- **Empty file**: written as a `.huff` file with zero frequency-table
  entries and `symbolCount = 0`; decompresses back to a zero-byte file.
- **Single unique character**: no tree traversal is meaningful (one
  leaf, no edges), so decoding simply emits that character
  `symbolCount` times, sidestepping any bit-counting ambiguity entirely.

## .huff Binary Format

```
[8 bytes]  uint64_t  magic           constant file-type sentinel
[8 bytes]  uint64_t  numEntries      number of (char, freq) pairs (0 = empty file)
numEntries × (1 + 8 bytes)           uint8_t character, uint64_t frequency
[8 bytes]  uint64_t  symbolCount     exact original byte count (authoritative for decode)
[8 bytes]  uint64_t  bitCount        number of meaningful encoded bits
ceil(bitCount/8) bytes               packed bit data, MSB-first, zero-padded
```

## Compilation

```bash
make
# or directly:
g++ -std=c++17 -Iinclude src/*.cpp main.cpp -o huffman.exe
```

Requires a C++17 compiler (tested with GCC). No external dependencies.

## Usage

```bash
./huffman.exe compress   sample.txt   sample.huff
./huffman.exe decompress sample.huff  output.txt
./huffman.exe roundtrip  sample.txt
```

## Example Output

```
[Step 1] Compressing...
[Compressor] Compressed 'data/sample.txt' -> 'output/compressed.huff'

[Huffman Codes]
Character     ASCII   Code
------------------------------------------------
SPACE         32      110
e             101     1110
...

[Step 2] Decompressing...
[Decompressor] Decompressed 'output/compressed.huff' -> 'output/decoded.txt'

========================================
          COMPRESSION STATISTICS
========================================
  Original Size                  N bytes
  Compressed Size                M bytes
  ...
========================================

[Step 3] Verifying...

========================================
  Verification : SUCCESS
  Original == Decoded
========================================
```

## Future Improvements

- Canonical Huffman codes for a more compact, standard header format
- Adaptive Huffman coding (single-pass, no upfront frequency table)
- Multi-file archive support
- Streaming compression for files too large to hold in memory
- Unit test suite (Google Test) covering the edge cases exercised
  manually during this review
