#include "Compressor.h"
#include "Decompressor.h"
#include "Statistics.h"
#include "Verifier.h"
#include <iostream>
#include <string>

static void printUsage(const char* programName) {
    std::cout << "\nUsage:\n";
    std::cout << "  " << programName << " compress   <input.txt>  <output.huff>\n";
    std::cout << "  " << programName << " decompress <input.huff> <output.txt>\n";
    std::cout << "  " << programName << " roundtrip  <input.txt>\n\n";
}

static void runRoundTrip(const std::string& inputPath) {
    const std::string compressedPath = "output/compressed.huff";
    const std::string decodedPath    = "output/decoded.txt";

    Compressor compressor;
    Decompressor decompressor;
    Statistics compStats;
    Statistics decompStats;

    std::cout << "\n[Step 1] Compressing...\n";
    compStats.startTimer();
    compressor.compress(inputPath, compressedPath);
    compStats.stopTimer();

    if (!compressor.getFrequencyMap().empty()) {
        std::cout << "\n[Huffman Codes]\n";
        compressor.getHuffmanTree().printCodes();
    } else {
        std::cout << "\n[Huffman Codes] (input file is empty — no codes to display)\n";
    }

    std::cout << "[Step 2] Decompressing...\n";
    decompStats.startTimer();
    decompressor.decompress(compressedPath, decodedPath);
    decompStats.stopTimer();

    Statistics stats;
    stats.recordFileSizes(inputPath, compressedPath);
    stats.printReport(compStats.getElapsedMs(), decompStats.getElapsedMs());

    std::cout << "[Step 3] Verifying...\n";
    Verifier verifier;
    const bool ok = verifier.verify(compressor.getOriginalContent(), decompressor.getDecodedContent());
    verifier.printResult(ok);

    if (!ok) {
        throw std::runtime_error("Round-trip verification failed.");
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    const std::string command = argv[1];

    try {
        if (command == "compress" && argc == 4) {
            Compressor c;
            Statistics s;
            s.startTimer();
            c.compress(argv[2], argv[3]);
            s.stopTimer();
            std::cout << "Done in " << s.getElapsedMs() << " ms\n";
            if (!c.getFrequencyMap().empty()) {
                c.getHuffmanTree().printCodes();
            }
        } else if (command == "decompress" && argc == 4) {
            Decompressor d;
            Statistics s;
            s.startTimer();
            d.decompress(argv[2], argv[3]);
            s.stopTimer();
            std::cout << "Done in " << s.getElapsedMs() << " ms\n";
        } else if (command == "roundtrip" && argc == 3) {
            runRoundTrip(argv[2]);
        } else {
            printUsage(argv[0]);
            return 1;
        }
    } catch (const std::exception& ex) {
        std::cerr << "\n[ERROR] " << ex.what() << "\n\n";
        return 1;
    }

    return 0;
}
