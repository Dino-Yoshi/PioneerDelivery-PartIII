#include "pioneer/Cleanup.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace pioneer {
namespace {

// Marks an impossible DP state. Valid scores are nonnegative, so -1 never collides
// with a real score. Code must test for this value before using a cell.
constexpr std::int32_t kUnreachable = -1;

// Largest DP table allowed, in bytes (256 MiB). Checked before the table is allocated.
constexpr std::size_t kMaxTableBytes = 256 * 1024 * 1024;

// A cell holds a deletion count, which can never exceed n = |source|. The table is
// capped at kMaxTableBytes, so n + 1 is at most kMaxTableBytes / 4, far below INT32_MAX.
// Four-byte cells therefore cannot overflow, and they halve memory against 8-byte cells.
using DpCell = std::int32_t;

// Flat index of cell (i, j) in a table with `columns` columns, where i is the number of
// source characters examined and j is the number of pattern characters matched.
std::size_t flatIndex(std::size_t i, std::size_t j, std::size_t columns) {
    return i * columns + j;
}

// Returns (n+1)*(m+1) after checking for arithmetic overflow and the byte limit.
// Runs before any table allocation.
std::size_t checkedTableCells(std::size_t n, std::size_t m) {
    constexpr std::size_t kMaxSize = std::numeric_limits<std::size_t>::max();
    if (n == kMaxSize || m == kMaxSize) {
        throw std::length_error("Cleanup: string length overflows the DP size computation");
    }
    const std::size_t rows = n + 1;
    const std::size_t columns = m + 1;
    if (rows > kMaxSize / columns) {
        throw std::length_error("Cleanup: (n+1)*(m+1) overflows the DP size computation");
    }
    const std::size_t cells = rows * columns;
    if (cells > kMaxTableBytes / sizeof(DpCell)) {
        throw std::length_error("Cleanup: DP table of (n+1)*(m+1) = " + std::to_string(cells) +
                                " cells of 4 bytes exceeds the 256 MiB limit");
    }
    return cells;
}

// Re-checks the recorded deletions without using the DP or the traceback logic.
// Each index must be in bounds, eligible, and strictly ascending, which also makes it
// unique. The count must match the optimum. Removing exactly these original positions
// must leave the pattern as a subsequence of the kept string.
void verifyDeletions(const CleanupInput& input, const std::vector<bool>& eligible,
                     const std::vector<std::size_t>& removed, DpCell optimum) {
    const std::size_t n = input.source.size();
    if (removed.size() != static_cast<std::size_t>(optimum)) {
        throw std::logic_error("Cleanup: deletion count does not match the DP optimum");
    }

    std::vector<bool> isRemoved(n, false);
    for (std::size_t k = 0; k < removed.size(); ++k) {
        const std::size_t index = removed[k];
        if (index >= n) {
            throw std::logic_error("Cleanup: deleted index is out of bounds");
        }
        if (!eligible[index]) {
            throw std::logic_error("Cleanup: deleted index is not eligible");
        }
        if (k > 0 && index <= removed[k - 1]) {
            throw std::logic_error("Cleanup: deleted indices are not strictly ascending");
        }
        isRemoved[index] = true;
    }

    // Keep original positions only. No index is renumbered.
    std::string kept;
    for (std::size_t position = 0; position < n; ++position) {
        if (!isRemoved[position]) {
            kept.push_back(input.source[position]);
        }
    }

    // Two-pointer subsequence check, written separately from the DP.
    std::size_t matched = 0;
    for (char character : kept) {
        if (matched < input.pattern.size() && character == input.pattern[matched]) {
            ++matched;
        }
    }
    if (matched != input.pattern.size()) {
        throw std::logic_error("Cleanup: removing the reported indices broke the pattern");
    }
}

// DP state: F[i][j] is the maximum number of deletions after examining the first i source
// characters while matching exactly the first j pattern characters in order. Unreachable
// means no alignment of pattern[0..j) inside source[0..i) exists. It is stored as
// kUnreachable and never added to or compared as a score.
//
// Base cases:
//   F[0][0] = 0.
//   F[0][j>0] = unreachable (no source characters examined, so nothing matched).
//   F[i][0] = number of eligible positions in source[0..i). This follows from the skip
//             transition below, since no pattern character needs matching.
//
// For i >= 1 and 0 <= j <= m, with c = source[i-1], take the largest valid candidate:
//   Skip (source[i-1] is not used for matching):
//     valid if F[i-1][j] is reachable. Candidate F[i-1][j] + (eligible[i-1] ? 1 : 0).
//     An ineligible skipped character stays in the kept string and earns nothing.
//   Match (source[i-1] matches pattern[j-1]):
//     valid if j >= 1, c == pattern[j-1], and F[i-1][j-1] is reachable.
//     Candidate F[i-1][j-1]. No deletion credit, because the character is needed.
//   F[i][j] is the larger valid candidate, or unreachable if neither is valid.
//
// Fill order: i from 1 to n in the outer loop, j from 0 to m in the inner loop. Row i-1 is
// finished before row i reads it. Time is O(n*m) and auxiliary space is O(n*m).
std::int32_t fillDeletionTable(const CleanupInput& input, const std::vector<bool>& eligible,
                               std::vector<DpCell>& table) {
    const std::size_t n = input.source.size();
    const std::size_t m = input.pattern.size();
    const std::size_t columns = m + 1;

    // Resource check runs before the table is allocated.
    table.assign(checkedTableCells(n, m), kUnreachable);
    table[flatIndex(0, 0, columns)] = 0;

    for (std::size_t i = 1; i <= n; ++i) {
        const std::size_t sourceIndex = i - 1;
        const DpCell skipGain = eligible[sourceIndex] ? 1 : 0;
        const char sourceCharacter = input.source[sourceIndex];

        for (std::size_t j = 0; j <= m; ++j) {
            DpCell best = kUnreachable;

            const DpCell skipFrom = table[flatIndex(i - 1, j, columns)];
            if (skipFrom != kUnreachable) {
                best = skipFrom + skipGain;
            }

            if (j >= 1 && sourceCharacter == input.pattern[j - 1]) {
                const DpCell matchFrom = table[flatIndex(i - 1, j - 1, columns)];
                if (matchFrom != kUnreachable && (best == kUnreachable || matchFrom > best)) {
                    best = matchFrom;
                }
            }

            table[flatIndex(i, j, columns)] = best;
        }
    }

    return table[flatIndex(n, m, columns)];
}

// Traceback from (n, m) to (0, 0). Tie policy: when a valid skip reproduces F[i][j], take
// the skip, even if a match would give the same score. Skip is preferred so the result is
// deterministic. The state moves only to reachable cells, so the unreachable checks never
// read a value that was not computed by the fill. An unreachable cell is an explicit
// error rather than a zero score, which would silently allow invalid alignments.
// Each step decreases i, so the loop ends at i = 0. The index list is then reversed into
// ascending order.
std::vector<std::size_t> traceDeletions(const CleanupInput& input, const std::vector<bool>& eligible,
                                        const std::vector<DpCell>& table) {
    const std::size_t n = input.source.size();
    const std::size_t m = input.pattern.size();
    const std::size_t columns = m + 1;

    std::vector<std::size_t> removed;
    std::size_t i = n;
    std::size_t j = m;
    while (i > 0 || j > 0) {
        const DpCell current = table[flatIndex(i, j, columns)];
        if (current == kUnreachable) {
            throw std::logic_error("Cleanup: traceback reached an unreachable state");
        }

        // Skip transition reproduces F[i][j] when F[i-1][j] is reachable and adding the
        // skip gain gives the same value.
        if (i > 0) {
            const DpCell skipFrom = table[flatIndex(i - 1, j, columns)];
            const DpCell skipGain = eligible[i - 1] ? 1 : 0;
            if (skipFrom != kUnreachable && skipFrom + skipGain == current) {
                if (eligible[i - 1]) {
                    removed.push_back(i - 1);
                }
                --i;
                continue;
            }
        }

        // Otherwise the match transition must be the one that produced F[i][j].
        const bool matchValid =
            i > 0 && j > 0 && input.source[i - 1] == input.pattern[j - 1] &&
            table[flatIndex(i - 1, j - 1, columns)] == current;
        if (!matchValid) {
            throw std::logic_error("Cleanup: traceback found no valid transition");
        }
        --i;
        --j;
    }

    std::reverse(removed.begin(), removed.end());
    return removed;
}

}  // namespace

CleanupResult solveCleanup(const CleanupInput& input) {
    const std::size_t n = input.source.size();

    std::vector<bool> eligible(n, false);
    for (std::size_t index : input.targetIndices) {
        if (index >= n) {
            throw std::invalid_argument("Cleanup: target index is out of bounds");
        }
        eligible[index] = true;
    }

    std::vector<DpCell> table;
    const DpCell optimum = fillDeletionTable(input, eligible, table);
    if (optimum == kUnreachable) {
        throw std::invalid_argument("Cleanup: pattern is not a subsequence of source");
    }

    CleanupResult result;
    result.removedIndices = traceDeletions(input, eligible, table);
    verifyDeletions(input, eligible, result.removedIndices, optimum);
    result.maximumDeletions = result.removedIndices.size();
    return result;
}

}  // namespace pioneer
