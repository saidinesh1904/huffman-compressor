#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <unordered_map>
#include <cstdint>

/**
 * FileManager — all disk I/O for the application goes through here.
 *
 * .huff binary file format (all integers little/native-endian, fixed width):
 *
 *   [8 bytes]  uint64_t  magic            ('H','U','F','F','1','\0','\0','\0' style guard - see MAGIC)
 *   [8 bytes]  uint64_t  numEntries       number of (char, freq) pairs in the table
 *   numEntries * (1 + 8 bytes)            uint8_t character, uint64_t frequency
 *   [8 bytes]  uint64_t  symbolCount      EXACT number of original bytes (authoritative)
 *   [8 bytes]  uint64_t  bitCount         number of meaningful encoded bits (informational,
 *                                          used only to size the packed buffer correctly)
 *   ceil(bitCount / 8) bytes              packed bit data, MSB-first, zero-padded in last byte
 *
 * symbolCount is what decoding actually relies on to know when to stop —
 * this makes the format robust to padding bits regardless of how many
 * trailing zero bits happen to also form valid tree paths.
 */
class FileManager {
public:
    using FrequencyMap = std::unordered_map<uint8_t, uint64_t>;

    FileManager() = default;

    std::string readTextFile(const std::string& path) const;
    void writeTextFile(const std::string& path, const std::string& content) const;

    /**
     * Write a compressed .huff file.
     * @param symbolCount  Exact number of original bytes (drives decode loop).
     */
    void writeCompressedFile(const std::string& path,
                             const FrequencyMap& freqMap,
                             const std::string& bitString,
                             uint64_t symbolCount) const;

    /**
     * Read a compressed .huff file.
     * @param freqMap     [out]
     * @param bitString   [out] unpacked '0'/'1' characters (length == bitCount stored in file,
     *                    i.e. padding already excluded)
     * @param symbolCount [out] exact original byte count
     */
    void readCompressedFile(const std::string& path,
                            FrequencyMap& freqMap,
                            std::string& bitString,
                            uint64_t& symbolCount) const;

    uint64_t getFileSize(const std::string& path) const;
    bool fileExists(const std::string& path) const;

private:
    static constexpr uint64_t kMagic = 0x31464655'5F6B'434EULL; // "NCk_UFF1" style sentinel
};

#endif // FILEMANAGER_H
