#ifndef VERIFIER_H
#define VERIFIER_H

#include <string>

/**
 * Verifier — confirms lossless round-trip by exact byte comparison.
 */
class Verifier {
public:
    Verifier() = default;

    bool verify(const std::string& original, const std::string& decoded) const;
    void printResult(bool success) const;
};

#endif // VERIFIER_H
