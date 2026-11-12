#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace pioneer {

struct CleanupInput {
    std::string source;   // nonempty lowercase ASCII
    std::string pattern;  // nonempty lowercase ASCII, subsequence of source
    // Strictly increasing original zero-based positions, each less than source.size().
    std::vector<std::size_t> targetIndices;
};

struct CleanupResult {
    std::size_t maximumDeletions = 0;
    // Ascending original zero-based indices of one optimal deletion set.
    std::vector<std::size_t> removedIndices;
};

// Precondition: the input parser has checked the alphabet, index bounds, index
// order, and the subsequence rule.
CleanupResult solveCleanup(const CleanupInput& input);

}  // namespace pioneer
