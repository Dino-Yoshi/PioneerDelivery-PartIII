// Independent oracle tests for Task C (event-log cleanup that keeps a pattern as a subsequence).
//
// The oracle enumerates every subset of the eligible original indices. For each subset it builds the
// remaining string in one pass over the source, skipping exactly the chosen indices (no renumbering),
// and keeps the subset when the pattern is still a subsequence of that string. It uses no prefix DP,
// predecessor table, or traceback, so it shares no code path with the production solver.
//
// Phase 1 checks the oracle against the expected values in docs/11_TEST_PLAN.md and the assignment.
// Phase 2 compares the production solver with the oracle and checks the reconstruction invariants of
// the production result with code written here.

#include "TestHarness.hpp"

#include "pioneer/Cleanup.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

using pioneer::CleanupInput;
using pioneer::CleanupResult;
using pioneer::testing::Checker;

constexpr std::size_t kMaxOracleEligible = 12;
constexpr std::size_t kMaxRandomSourceLength = 12;
constexpr std::uint64_t kRandomAlphabetSize = 3;  // letters 'a', 'b', 'c' give repeated symbols
constexpr std::uint64_t kFirstSeed = 1;
constexpr std::uint64_t kLastSeed = 25;

constexpr std::uint64_t kLcgMultiplier = 6364136223846793005ULL;
constexpr std::uint64_t kLcgIncrement = 1442695040888963407ULL;

// Small deterministic generator (Knuth's MMIX constants). Its output depends only on the seed, so a
// failing seed reproduces the same instance on every machine.
class LinearCongruentialGenerator {
public:
    explicit LinearCongruentialGenerator(std::uint64_t seed) : state_(seed) {}

    // Returns a value in [0, bound). The bound must be positive.
    std::uint64_t nextBelow(std::uint64_t bound) {
        state_ = state_ * kLcgMultiplier + kLcgIncrement;
        // The low bits of a power-of-two LCG repeat quickly, so the high bits are used.
        return (state_ >> 33) % bound;
    }

private:
    std::uint64_t state_;
};

struct NamedCase {
    std::string caseId;
    CleanupInput input;
    std::size_t expectedMaximumDeletions;
};

// Named cases C-01..C-10 from docs/11_TEST_PLAN.md. C-01..C-04 are the four official examples.
std::vector<NamedCase> namedCleanupCases() {
    return {
        // C-01: deleting index 1 or index 2 is optimal. Index 0 must stay.
        {"C-01", {"abbaa", "aba", {0, 1, 2}}, 1},
        // C-02: both eligible ends can go, leaving "cd".
        {"C-02", {"bcda", "d", {0, 3}}, 2},
        // C-03: every character is needed to keep "dda".
        {"C-03", {"dda", "dda", {0, 1, 2}}, 0},
        // C-04: deleting indices 2 and 3 leaves "yeykyded", which still contains "yeyyd".
        {"C-04", {"yeyeykyded", "yeyyd", {0, 2, 3, 4}}, 2},
        // C-05: a greedy earliest match of "aba" uses indices 0 to 2 and blocks every deletion. Deleting
        // indices 0 to 2 leaves the final "aba", so the optimum is 3.
        {"C-05", {"abaaba", "aba", {0, 1, 2}}, 3},
        // C-06: no eligible indices, so nothing can be removed.
        {"C-06", {"abc", "ac", {}}, 0},
        // C-07: any two of the four equal characters can be deleted, but three would leave one "a".
        {"C-07", {"aaaa", "aa", {0, 1, 2, 3}}, 2},
        // C-08: deleting the eligible prefix "xyz" leaves "abc".
        {"C-08", {"xyzabc", "abc", {0, 1, 2}}, 3},
        // C-09: the eligible boundary characters x and y can go. The ineligible b and c stay.
        {"C-09", {"xabcy", "ac", {0, 4}}, 2},
        // C-10: the pattern equals the source, so no character can go.
        {"C-10", {"abc", "abc", {0, 1, 2}}, 0},
    };
}

std::string describeInput(const CleanupInput& input) {
    std::ostringstream text;
    text << "source=\"" << input.source << "\" pattern=\"" << input.pattern << "\" targetIndices=[";
    for (std::size_t i = 0; i < input.targetIndices.size(); ++i) {
        text << (i == 0 ? "" : ",") << input.targetIndices[i];
    }
    text << "]";
    return text.str();
}

// Two-pointer subsequence test: walk the text once and advance through the pattern whenever the next
// pattern character is found.
bool isSubsequence(const std::string& pattern, const std::string& text) {
    std::size_t matched = 0;
    for (std::size_t i = 0; i < text.size() && matched < pattern.size(); ++i) {
        if (text[i] == pattern[matched]) {
            ++matched;
        }
    }
    return matched == pattern.size();
}

// Builds the string left after deleting the marked positions. Positions are not renumbered; the
// remaining characters keep their order.
std::string keptCharacters(const std::string& source, const std::vector<bool>& removed) {
    std::string kept;
    for (std::size_t i = 0; i < source.size(); ++i) {
        if (!removed[i]) {
            kept.push_back(source[i]);
        }
    }
    return kept;
}

// Returns the largest number of deletions from eligible indices that keeps the pattern a subsequence.
std::size_t oracleMaximumDeletions(const CleanupInput& input) {
    const std::size_t eligibleCount = input.targetIndices.size();
    if (eligibleCount > kMaxOracleEligible) {
        throw std::invalid_argument("cleanup oracle is limited to 12 eligible indices");
    }
    std::size_t best = 0;
    const std::uint32_t subsetCount = std::uint32_t{1} << eligibleCount;
    for (std::uint32_t mask = 0; mask < subsetCount; ++mask) {
        std::vector<bool> removed(input.source.size(), false);
        std::size_t removedCount = 0;
        for (std::size_t b = 0; b < eligibleCount; ++b) {
            if (((mask >> b) & 1u) != 0) {
                removed[input.targetIndices[b]] = true;
                ++removedCount;
            }
        }
        if (removedCount > best &&
            isSubsequence(input.pattern, keptCharacters(input.source, removed))) {
            best = removedCount;
        }
    }
    return best;
}

// Checks the reconstruction invariants of a production result. Returns an empty string when all
// invariants hold, otherwise a description of the first violation.
std::string cleanupViolation(const CleanupInput& input, const CleanupResult& result) {
    const std::size_t length = input.source.size();
    if (result.removedIndices.size() != result.maximumDeletions) {
        return "removed index count " + std::to_string(result.removedIndices.size()) +
               " differs from reported maximum " + std::to_string(result.maximumDeletions);
    }

    std::vector<bool> removed(length, false);
    for (std::size_t i = 0; i < result.removedIndices.size(); ++i) {
        const std::size_t index = result.removedIndices[i];
        if (index >= length) {
            return "removed index " + std::to_string(index) + " is out of bounds";
        }
        // Strictly ascending order also rejects duplicates.
        if (i > 0 && index <= result.removedIndices[i - 1]) {
            return "removed indices are not strictly ascending";
        }
        if (std::find(input.targetIndices.begin(), input.targetIndices.end(), index) ==
            input.targetIndices.end()) {
            return "removed index " + std::to_string(index) + " is not eligible";
        }
        removed[index] = true;
    }

    // Verify the claim directly: delete exactly the reported positions and test the pattern again.
    if (!isSubsequence(input.pattern, keptCharacters(input.source, removed))) {
        return "remaining string no longer contains the pattern after the reported deletions";
    }
    return "";
}

CleanupInput randomCleanupInput(LinearCongruentialGenerator& rng) {
    CleanupInput input;
    const std::size_t sourceLength =
        1 + static_cast<std::size_t>(rng.nextBelow(kMaxRandomSourceLength));
    for (std::size_t i = 0; i < sourceLength; ++i) {
        input.source.push_back(static_cast<char>('a' + rng.nextBelow(kRandomAlphabetSize)));
    }

    // The pattern is a random subsequence of the source: keep each position with probability 1/2.
    // If that keeps nothing, keep one random position so the pattern is nonempty.
    for (std::size_t i = 0; i < sourceLength; ++i) {
        if (rng.nextBelow(2) == 0) {
            input.pattern.push_back(input.source[i]);
        }
    }
    if (input.pattern.empty()) {
        input.pattern.push_back(input.source[rng.nextBelow(sourceLength)]);
    }

    for (std::size_t i = 0; i < sourceLength; ++i) {
        if (rng.nextBelow(2) == 0) {
            input.targetIndices.push_back(i);
        }
    }
    return input;
}

void runOracleSelfChecks(Checker& check) {
    for (const NamedCase& namedCase : namedCleanupCases()) {
        const std::size_t oracle = oracleMaximumDeletions(namedCase.input);
        check.expect(oracle == namedCase.expectedMaximumDeletions,
                     namedCase.caseId + ": oracle maximum " + std::to_string(oracle) +
                         " differs from expected " +
                         std::to_string(namedCase.expectedMaximumDeletions));
    }
}

void checkProductionAgainstOracle(Checker& check, const std::string& label,
                                  const CleanupInput& input) {
    const std::size_t oracle = oracleMaximumDeletions(input);
    const CleanupResult result = pioneer::solveCleanup(input);
    const std::string instance = describeInput(input);

    check.expect(result.maximumDeletions == oracle,
                 label + ": production maximum " + std::to_string(result.maximumDeletions) +
                     " differs from oracle " + std::to_string(oracle) + "; instance: " + instance);
    const std::string violation = cleanupViolation(input, result);
    check.expect(violation.empty(), label + ": " + violation + "; instance: " + instance);
}

// Phase 2 does not catch exceptions. A stub that throws std::logic_error stops this phase, and main
// reports the message as a failed check.
void runProductionComparison(Checker& check) {
    for (const NamedCase& namedCase : namedCleanupCases()) {
        checkProductionAgainstOracle(check, namedCase.caseId, namedCase.input);
    }
    for (std::uint64_t seed = kFirstSeed; seed <= kLastSeed; ++seed) {
        LinearCongruentialGenerator rng(seed);
        checkProductionAgainstOracle(check, "seed " + std::to_string(seed), randomCleanupInput(rng));
    }
}

}  // namespace

int main() {
    Checker selfCheck;
    runOracleSelfChecks(selfCheck);
    const int selfCheckFailures = selfCheck.report("ORACLE SELF-CHECKS");

    Checker production;
    try {
        runProductionComparison(production);
    } catch (const std::exception& error) {
        production.expect(false, std::string("production call threw: ") + error.what());
    }
    const int productionFailures = production.report("PRODUCTION COMPARISON");

    return (selfCheckFailures == 0 && productionFailures == 0) ? 0 : 1;
}
