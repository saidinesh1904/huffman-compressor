#include "Statistics.h"
#include "FileManager.h"
#include <iostream>
#include <iomanip>

void Statistics::startTimer() { start_ = Clock::now(); }
void Statistics::stopTimer()  { stop_  = Clock::now(); }

double Statistics::getElapsedMs() const {
    return std::chrono::duration<double, std::milli>(stop_ - start_).count();
}

void Statistics::recordFileSizes(const std::string& originalPath, const std::string& compressedPath) {
    FileManager fm;
    originalSize_   = fm.getFileSize(originalPath);
    compressedSize_ = fm.getFileSize(compressedPath);

    ratio_ = (originalSize_ > 0)
        ? (1.0 - static_cast<double>(compressedSize_) / static_cast<double>(originalSize_)) * 100.0
        : 0.0;
}

void Statistics::printReport(double compressionMs, double decompressionMs) const {
    const int64_t bytesSaved = static_cast<int64_t>(originalSize_) - static_cast<int64_t>(compressedSize_);

    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "          COMPRESSION STATISTICS\n";
    std::cout << "========================================\n";
    std::cout << std::left << std::setw(24) << "  Original Size"
              << std::right << std::setw(10) << originalSize_ << " bytes\n";
    std::cout << std::left << std::setw(24) << "  Compressed Size"
              << std::right << std::setw(10) << compressedSize_ << " bytes\n";
    std::cout << std::left << std::setw(24) << "  Bytes Saved"
              << std::right << std::setw(10) << bytesSaved << " bytes\n";
    std::cout << std::left << std::setw(24) << "  Compression Ratio"
              << std::right << std::setw(9) << std::fixed << std::setprecision(2) << ratio_ << " %\n";
    std::cout << std::left << std::setw(24) << "  Compression Time"
              << std::right << std::setw(9) << std::fixed << std::setprecision(3) << compressionMs << " ms\n";
    std::cout << std::left << std::setw(24) << "  Decompression Time"
              << std::right << std::setw(9) << std::fixed << std::setprecision(3) << decompressionMs << " ms\n";
    std::cout << "========================================\n\n";
}
