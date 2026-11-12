// Cleanup tests: the four official examples, the named C cases, edge cases, an exhaustive
// small-string sweep, and the table-size limit.
//
// Every successful result is checked by code in this file that does not use the production
// DP or traceback: index validity, eligibility, ordering, the reported count, and a kept-string
// subsequence check. For small inputs, the optimum is also compared with an exhaustive
// enumeration of eligible subsets. The tests never require one particular index set,
// because several optimal deletion sets can be valid.

#include "TestHarness.hpp"

#include "pioneer/Cleanup.hpp"

#include <cstddef>
#include <exception>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

using pioneer::CleanupInput;
using pioneer::CleanupResult;
using pioneer::solveCleanup;
using pioneer::testing::Checker;

// Largest eligible-set size for which the exhaustive subset oracle runs (2^12 subsets).
constexpr std::size_t kMaxBruteForceEligible = 12;

CleanupInput makeInput(const std::string& source, const std::string& pattern,
                       std::vector<std::size_t> targets) {
    CleanupInput input;
    input.source = source;
    input.pattern = pattern;
    input.targetIndices = std::move(targets);
    return input;
}

// Test-local two-pointer subsequence check.
bool containsSubsequence(const std::string& text, const std::string& pattern) {
    std::size_t matched = 0;
    for (char character : text) {
        if (matched < pattern.size() && character == pattern[matched]) {
            ++matched;
        }
    }
    return matched == pattern.size();
}

// The source with exactly the listed original positions removed. Positions are not renumbered
// in the input; only the surviving characters are concatenated.
std::string sourceWithout(const std::string& source, const std::vector<std::size_t>& removed) {
    std::vector<bool> gone(source.size(), false);
    for (std::size_t index : removed) {
        if (index < source.size()) {
            gone[index] = true;
        }
    }
    std::string kept;
    for (std::size_t position = 0; position < source.size(); ++position) {
        if (!gone[position]) {
            kept.push_back(source[position]);
        }
    }
    return kept;
}

// Test oracle: tries every subset of the eligible positions and returns the largest number
// of removals that keeps pattern a subsequence. Shares no code with the production solver.
std::size_t bruteForceOptimum(const CleanupInput& input) {
    const std::size_t eligibleCount = input.targetIndices.size();
    std::size_t best = 0;
    for (std::size_t mask = 0; mask < (std::size_t{1} << eligibleCount); ++mask) {
        std::vector<std::size_t> removed;
        for (std::size_t bit = 0; bit < eligibleCount; ++bit) {
            if ((mask >> bit) & 1) {
                removed.push_back(input.targetIndices[bit]);
            }
        }
        if (removed.size() > best &&
            containsSubsequence(sourceWithout(input.source, removed), input.pattern)) {
            best = removed.size();
        }
    }
    return best;
}

// Checks every invariant that holds for any optimal answer, without fixing the index set.
void checkResultInvariants(Checker& check, const std::string& label, const CleanupInput& input,
                           const CleanupResult& result) {
    const std::vector<std::size_t>& removed = result.removedIndices;
    const std::set<std::size_t> targets(input.targetIndices.begin(), input.targetIndices.end());

    bool ascendingAndUnique = true;
    bool inBounds = true;
    bool allEligible = true;
    for (std::size_t k = 0; k < removed.size(); ++k) {
        if (removed[k] >= input.source.size()) {
            inBounds = false;
        }
        if (targets.count(removed[k]) == 0) {
            allEligible = false;
        }
        if (k > 0 && removed[k] <= removed[k - 1]) {
            ascendingAndUnique = false;
        }
    }

    check.expect(ascendingAndUnique, label + ": removed indices are strictly ascending and unique");
    check.expect(inBounds, label + ": removed indices are in bounds");
    check.expect(allEligible, label + ": every removed index is eligible");
    check.expect(removed.size() == result.maximumDeletions,
                 label + ": removed index count equals the reported maximum");
    check.expect(containsSubsequence(sourceWithout(input.source, removed), input.pattern),
                 label + ": kept string still contains the pattern as a subsequence");
}

std::optional<CleanupResult> runSolver(Checker& check, const std::string& label,
                                       const CleanupInput& input) {
    try {
        return solveCleanup(input);
    } catch (const std::exception& error) {
        check.expect(false, label + ": unexpected exception: " + error.what());
        return std::nullopt;
    }
}

// Runs one case: checks the expected optimum, the result invariants, and (for small
// eligible sets) agreement with the exhaustive oracle.
void checkCase(Checker& check, const std::string& label, const CleanupInput& input,
               std::size_t expectedOptimum) {
    const std::optional<CleanupResult> result = runSolver(check, label, input);
    if (!result) {
        return;
    }
    check.expect(result->maximumDeletions == expectedOptimum,
                 label + ": optimum is " + std::to_string(expectedOptimum) + " (got " +
                     std::to_string(result->maximumDeletions) + ")");
    checkResultInvariants(check, label, input, *result);
    if (input.targetIndices.size() <= kMaxBruteForceEligible) {
        check.expect(bruteForceOptimum(input) == expectedOptimum,
                     label + ": exhaustive subset oracle agrees with the expected optimum");
    }
}

void testOfficialExamples(Checker& check) {
    checkCase(check, "official example C-01 abbaa/aba",
              makeInput("abbaa", "aba", {0, 1, 2}), 1);
    checkCase(check, "official example C-02 bcda/d", makeInput("bcda", "d", {0, 3}), 2);
    checkCase(check, "official example C-03 dda/dda", makeInput("dda", "dda", {0, 1, 2}), 0);
    checkCase(check, "official example C-04 yeyeykyded/yeyyd",
              makeInput("yeyeykyded", "yeyyd", {0, 2, 3, 4}), 2);
}

void testNamedCases(Checker& check) {
    // C-05 is the case where greedy earliest alignment fails: the answer is 3, not 1 or 2.
    checkCase(check, "named case C-05 abaaba/aba", makeInput("abaaba", "aba", {0, 1, 2}), 3);
    checkCase(check, "named case C-06 abc/ac, no eligible", makeInput("abc", "ac", {}), 0);
    checkCase(check, "named case C-07 aaaa/aa, all eligible", makeInput("aaaa", "aa", {0, 1, 2, 3}),
              2);
    checkCase(check, "named case C-08 xyzabc/abc", makeInput("xyzabc", "abc", {0, 1, 2}), 3);
    checkCase(check, "named case C-09 xabcy/ac, boundary deletions",
              makeInput("xabcy", "ac", {0, 4}), 2);
    checkCase(check, "named case C-10 abc/abc, pattern equals source",
              makeInput("abc", "abc", {0, 1, 2}), 0);
}

void testEdgeCases(Checker& check) {
    checkCase(check, "edge: no eligible positions, longer source", makeInput("abcabc", "abc", {}), 0);
    checkCase(check, "edge: all eligible, pattern is one character", makeInput("bcda", "d", {0, 1, 2, 3}),
              3);
    checkCase(check, "edge: pattern equals source, all eligible", makeInput("abcd", "abcd", {0, 1, 2, 3}),
              0);
    checkCase(check, "edge: repeated characters, several alignments",
              makeInput("abababab", "abab", {0, 1, 2, 3, 4, 5, 6, 7}), 4);
    checkCase(check, "edge: repeated characters, only middle eligible",
              makeInput("aabbaabb", "ab", {1, 2, 3, 4, 5, 6}), 6);
    checkCase(check, "edge: eligible index 0 only", makeInput("bab", "b", {0}), 1);
    checkCase(check, "edge: eligible last index only", makeInput("bab", "b", {2}), 1);
    checkCase(check, "edge: eligible first and last, cannot delete both", makeInput("bab", "b", {0, 2}), 1);
    checkCase(check, "edge: two a's needed out of four", makeInput("aaba", "aa", {0, 1, 2, 3}), 2);
    checkCase(check, "edge: ineligible prefix stays, eligible prefix removed",
              makeInput("abcabc", "abc", {0, 1}), 2);
    checkCase(check, "edge: single character source and pattern, eligible",
              makeInput("a", "a", {0}), 0);
}

// Builds a string of `length` characters over {a, b}; bit k selects the k-th character.
std::string bitsToString(std::size_t bits, std::size_t length) {
    std::string text;
    for (std::size_t position = 0; position < length; ++position) {
        text.push_back(((bits >> position) & 1) ? 'b' : 'a');
    }
    return text;
}

// Compares the solver with the exhaustive oracle on every source of length 1..5 over {a, b},
// every subsequence pattern of that source, and every eligible-index set.
void testExhaustiveSmallSweep(Checker& check) {
    constexpr std::size_t kMaxLength = 5;
    std::size_t casesRun = 0;
    for (std::size_t sourceLength = 1; sourceLength <= kMaxLength; ++sourceLength) {
        for (std::size_t sourceBits = 0; sourceBits < (std::size_t{1} << sourceLength); ++sourceBits) {
            const std::string source = bitsToString(sourceBits, sourceLength);
            for (std::size_t patternLength = 1; patternLength <= sourceLength; ++patternLength) {
                for (std::size_t patternBits = 0; patternBits < (std::size_t{1} << patternLength);
                     ++patternBits) {
                    const std::string pattern = bitsToString(patternBits, patternLength);
                    if (!containsSubsequence(source, pattern)) {
                        continue;
                    }
                    for (std::size_t eligibleMask = 0; eligibleMask < (std::size_t{1} << sourceLength);
                         ++eligibleMask) {
                        std::vector<std::size_t> targets;
                        for (std::size_t position = 0; position < sourceLength; ++position) {
                            if ((eligibleMask >> position) & 1) {
                                targets.push_back(position);
                            }
                        }
                        const CleanupInput input = makeInput(source, pattern, targets);
                        const std::string label = "sweep source=" + source + " pattern=" + pattern +
                                                  " eligibleMask=" + std::to_string(eligibleMask);
                        ++casesRun;
                        const std::optional<CleanupResult> result = runSolver(check, label, input);
                        if (!result) {
                            continue;
                        }
                        const std::size_t expected = bruteForceOptimum(input);
                        check.expect(result->maximumDeletions == expected,
                                     label + ": optimum " + std::to_string(expected) + " (got " +
                                         std::to_string(result->maximumDeletions) + ")");
                        checkResultInvariants(check, label, input, *result);
                    }
                }
            }
        }
    }
    check.expect(casesRun > 0, "sweep: at least one case was generated");
    std::cout << "sweep: " << casesRun << " generated cases checked\n";
}

// A 9000 x 8000 pattern/source pair needs 9001 * 8001 cells of 4 bytes, about 288 MB, which is
// over the 256 MiB limit. The solver must reject it with std::length_error from the size check,
// before the table is allocated.
void testTableLimit(Checker& check) {
    const CleanupInput oversized = makeInput(std::string(9000, 'a'), std::string(8000, 'a'), {});
    check.expectThrows<std::length_error>([&oversized] { solveCleanup(oversized); }, "exceeds",
                                          "table limit: 9001 x 8001 cells exceed 256 MiB and throw "
                                          "std::length_error");
}

}  // namespace

int main() {
    Checker check;
    testOfficialExamples(check);
    testNamedCases(check);
    testEdgeCases(check);
    testExhaustiveSmallSweep(check);
    testTableLimit(check);
    return check.report("cleanup_tests") == 0 ? 0 : 1;
}
