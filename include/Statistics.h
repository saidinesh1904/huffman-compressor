#ifndef STATISTICS_H
#define STATISTICS_H

#include <string>
#include <chrono>
#include <cstdint>

/**
 * Statistics — timing and size-based compression metrics.
 */
class Statistics {
public:
    Statistics() = default;

    void startTimer();
    void stopTimer();
    double getElapsedMs() const;

    void recordFileSizes(const std::string& originalPath, const std::string& compressedPath);
    void printReport(double compressionMs, double decompressionMs) const;

    uint64_t getOriginalSize() const noexcept { return originalSize_; }
    uint64_t getCompressedSize() const noexcept { return compressedSize_; }
    double getCompressionRatio() const noexcept { return ratio_; }

private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    TimePoint start_{};
    TimePoint stop_{};

    uint64_t originalSize_ = 0;
    uint64_t compressedSize_ = 0;
    double ratio_ = 0.0;
};

#endif // STATISTICS_H
