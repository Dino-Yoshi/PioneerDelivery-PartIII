// Weighted interval scheduling tests (Task A).
//
// Named cases follow docs/11_TEST_PLAN.md. Every successful result is also checked by
// verifyResult, which uses its own pairwise overlap test and sum. Brute-force subset
// enumeration serves as an independent reference for small inputs. Neither helper calls
// production code, and tests do not require one particular ID set when ties exist.

#include "TestHarness.hpp"

#include "pioneer/Scheduling.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using pioneer::DeliveryInterval;
using pioneer::SchedulingResult;
using pioneer::solveScheduling;
using pioneer::testing::Checker;

constexpr std::int64_t kInt64Max = std::numeric_limits<std::int64_t>::max();

DeliveryInterval delivery(const std::string& id, std::int64_t start, std::int64_t finish, std::int64_t profitCents) {
    DeliveryInterval result;
    result.id = id;
    result.start = start;
    result.finish = finish;
    result.profitCents = profitCents;
    return result;
}

// Checks the invariants that every correct answer must satisfy, independently of the
// production code: known unique IDs, chronological order, pairwise compatibility, and a
// profit sum equal to the reported maximum.
void verifyResult(Checker& check, const std::string& name, const std::vector<DeliveryInterval>& input,
                  const SchedulingResult& result) {
    std::map<std::string, const DeliveryInterval*> byId;
    for (const DeliveryInterval& item : input) {
        byId[item.id] = &item;
    }

    std::set<std::string> seen;
    std::vector<const DeliveryInterval*> chosen;
    bool allKnown = true;
    bool allUnique = true;
    for (const std::string& id : result.selectedIds) {
        const auto found = byId.find(id);
        if (found == byId.end()) {
            allKnown = false;
            continue;
        }
        if (!seen.insert(id).second) {
            allUnique = false;
        }
        chosen.push_back(found->second);
    }
    check.expect(allKnown, name + ": every selected id comes from the input");
    check.expect(allUnique, name + ": selected ids are unique");

    bool chronological = true;
    bool compatible = true;
    std::int64_t profitSum = 0;
    for (std::size_t a = 0; a < chosen.size(); ++a) {
        profitSum += chosen[a]->profitCents;
        if (a + 1 < chosen.size() && chosen[a]->finish > chosen[a + 1]->finish) {
            chronological = false;
        }
        // Each later selection must start at or after this one finishes. Because start < finish
        // for every delivery, this also orders the selected intervals in time.
        for (std::size_t b = a + 1; b < chosen.size(); ++b) {
            if (chosen[a]->finish > chosen[b]->start) {
                compatible = false;
            }
        }
    }
    check.expect(chronological, name + ": selected ids are in chronological order");
    check.expect(compatible, name + ": selected deliveries do not overlap");
    check.expect(profitSum == result.maximumProfitCents,
                 name + ": sum of selected profits equals the reported maximum");
}

// Returns the solver's result, or nullopt after recording an unexpected exception as a failure.
std::optional<SchedulingResult> trySolve(Checker& check, const std::string& name,
                                         const std::vector<DeliveryInterval>& input) {
    try {
        return solveScheduling(input);
    } catch (const std::exception& error) {
        check.expect(false, name + ": unexpected exception: " + error.what());
        return std::nullopt;
    }
}

// Solves an instance, checks the optimum value exactly, and verifies the selection.
void expectOptimum(Checker& check, const std::string& name, const std::vector<DeliveryInterval>& input,
                   std::int64_t expectedCents) {
    const std::optional<SchedulingResult> result = trySolve(check, name, input);
    if (!result) {
        return;
    }
    check.expect(result->maximumProfitCents == expectedCents,
                 name + ": optimum is " + std::to_string(expectedCents) + " cents, got " +
                     std::to_string(result->maximumProfitCents));
    verifyResult(check, name, input, *result);
}

// Exhaustive reference for n <= 10. It tries every subset, keeps only pairwise compatible
// subsets, and returns the largest total profit. It shares no code with the solver.
std::int64_t bruteForceOptimum(const std::vector<DeliveryInterval>& input) {
    const std::size_t n = input.size();
    std::int64_t best = 0;
    for (std::size_t mask = 0; mask < (std::size_t{1} << n); ++mask) {
        bool compatible = true;
        std::int64_t total = 0;
        for (std::size_t a = 0; a < n && compatible; ++a) {
            if ((mask & (std::size_t{1} << a)) == 0) {
                continue;
            }
            total += input[a].profitCents;
            for (std::size_t b = a + 1; b < n; ++b) {
                if ((mask & (std::size_t{1} << b)) == 0) {
                    continue;
                }
                const bool aBeforeB = input[a].finish <= input[b].start;
                const bool bBeforeA = input[b].finish <= input[a].start;
                if (!aBeforeB && !bBeforeA) {
                    compatible = false;
                    break;
                }
            }
        }
        if (compatible && total > best) {
            best = total;
        }
    }
    return best;
}

// Small deterministic generator (64-bit LCG). It avoids std distributions, whose output can
// differ between standard library implementations, so every run sees the same instances.
class FixedRandom {
public:
    explicit FixedRandom(std::uint64_t seed) : state_(seed) {}

    // Returns a value in [low, high] inclusive.
    std::int64_t between(std::int64_t low, std::int64_t high) {
        state_ = state_ * 6364136223846793005ULL + 1442695040888963407ULL;
        const std::uint64_t span = static_cast<std::uint64_t>(high - low) + 1;
        return low + static_cast<std::int64_t>((state_ >> 33) % span);
    }

private:
    std::uint64_t state_;
};

void testNamedCases(Checker& check) {
    // A-01: official example from the assignment. Optimum B+D+F = 60+70+50.
    expectOptimum(check, "A-01 official example",
                  {delivery("A", 0, 3, 5000), delivery("B", 1, 4, 6000), delivery("C", 3, 5, 4000),
                   delivery("D", 4, 7, 7000), delivery("E", 5, 8, 5000), delivery("F", 7, 9, 5000)},
                  18000);

    // A-02: empty delivery set.
    const std::optional<SchedulingResult> empty = trySolve(check, "A-02 empty", {});
    if (empty) {
        check.expect(empty->maximumProfitCents == 0 && empty->selectedIds.empty(),
                     "A-02 empty: profit 0 and no ids");
    }

    // A-03: endpoint touching. A finishes at 2 and B starts at 2, so both fit.
    expectOptimum(check, "A-03 endpoint touching", {delivery("A", 0, 2, 3000), delivery("B", 2, 5, 4000)}, 7000);

    // A-04: every interval overlaps the others. Only the most valuable one can be chosen.
    expectOptimum(check, "A-04 all overlap",
                  {delivery("A", 0, 3, 5000), delivery("B", 1, 4, 6000), delivery("C", 2, 5, 4000)}, 6000);

    // A-05: two identical intervals. Exactly one may be chosen, and either one is valid.
    expectOptimum(check, "A-05 equal intervals tie", {delivery("X", 0, 2, 5000), delivery("Y", 0, 2, 5000)}, 5000);

    // A-06: greedy earliest finish picks A and C for 20 cents. Optimum is B alone.
    expectOptimum(check, "A-06 greedy counterexample",
                  {delivery("A", 0, 2, 1000), delivery("B", 0, 3, 10000), delivery("C", 2, 4, 1000)}, 10000);

    // A-07: every profit is zero. The optimum is 0, and the chosen set may be anything valid.
    expectOptimum(check, "A-07 all zero profits",
                  {delivery("A", 0, 2, 0), delivery("B", 1, 3, 0), delivery("C", 2, 4, 0)}, 0);

    // A-08: negative times, several deliveries with the same finish, shuffled input order.
    // Best choice is V(-20,-10) + X(-10,-5) + Z(-5,-1) = 1000 + 300 + 400.
    const std::vector<DeliveryInterval> shuffled = {
        delivery("Z", -5, -1, 40000),
        delivery("W", -6, -5, 15000),
        delivery("V", -20, -10, 100000),
        delivery("Y", -7, -5, 20000),
        delivery("X", -10, -5, 30000),
    };
    expectOptimum(check, "A-08 negative times, equal finish, shuffled", shuffled, 170000);
    check.expect(bruteForceOptimum(shuffled) == 170000, "A-08: brute force agrees with 170000 cents");
}

void testPredecessorEdges(Checker& check) {
    // The predecessor search for each delivery must not find the delivery itself or any later
    // one. Here every delivery touches the next, so each p(i) is i-1 and all three can be chosen.
    expectOptimum(check, "touching chain shuffled",
                  {delivery("C", 2, 3, 100), delivery("A", 0, 1, 100), delivery("B", 1, 2, 100)}, 300);

    // X and Y share finish time 5 and overlap each other, so at most one of them can be chosen.
    // Z starts at 5, so it is compatible with either. Best is Y + Z = 200 + 50 = 250;
    // X + Z = 150 and Y alone = 200 are both worse.
    expectOptimum(check, "equal finish, different starts",
                  {delivery("X", 0, 5, 100), delivery("Y", 3, 5, 200), delivery("Z", 5, 6, 50)}, 250);

    // C(5,10) and B(10,11) touch at 10, so C + B = 900 + 100 = 1000 beats A alone (700) and A + B (800).
    expectOptimum(check, "gap and exact touch",
                  {delivery("A", 0, 10, 700), delivery("B", 10, 11, 100), delivery("C", 5, 10, 900)}, 1000);
}

void testLongChain(Checker& check) {
    // 2000 back-to-back intervals [k, k+1] with varying profits. Every interval is compatible
    // with all others, so the optimum is the sum of all profits.
    constexpr std::int64_t kChainLength = 2000;
    std::vector<DeliveryInterval> chain;
    std::int64_t expectedSum = 0;
    for (std::int64_t k = 0; k < kChainLength; ++k) {
        const std::int64_t profit = (k % 7) + 1;
        expectedSum += profit;
        chain.push_back(delivery("D" + std::to_string(k), k, k + 1, profit));
    }
    // Shuffle with a fixed reversal so the sort does real work.
    std::reverse(chain.begin(), chain.end());
    const std::optional<SchedulingResult> result = trySolve(check, "long chain", chain);
    if (result) {
        check.expect(result->maximumProfitCents == expectedSum,
                     "long chain: optimum equals the sum of all profits");
        check.expect(result->selectedIds.size() == static_cast<std::size_t>(kChainLength),
                     "long chain: every delivery is selected");
        verifyResult(check, "long chain", chain, *result);
    }
}

void testOverflow(Checker& check) {
    // Two profits whose sum exceeds INT64_MAX. The DP must throw rather than wrap around.
    const std::vector<DeliveryInterval> overflowing = {delivery("A", 0, 1, kInt64Max), delivery("B", 1, 2, 1)};
    check.expectThrows<std::overflow_error>([&overflowing] { solveScheduling(overflowing); },
                                            "signed 64-bit", "overflow: profit sum above INT64_MAX throws");

    // Boundary case: the sum is exactly INT64_MAX, so no error is raised.
    const std::vector<DeliveryInterval> boundary = {delivery("A", 0, 1, kInt64Max - 1), delivery("B", 1, 2, 1)};
    expectOptimum(check, "overflow boundary exactly INT64_MAX", boundary, kInt64Max);
}

void testInvalidInput(Checker& check) {
    check.expectThrows<std::invalid_argument>([] { solveScheduling({delivery("A", 3, 3, 1)}); },
                                              "start < finish", "invalid: start equal to finish throws");
    check.expectThrows<std::invalid_argument>([] { solveScheduling({delivery("A", 0, 1, -1)}); },
                                              "negative profit", "invalid: negative profit throws");
}

void testRandomAgainstBruteForce(Checker& check) {
    // Deterministic random instances with n <= 10. The solver result must match exhaustive
    // enumeration exactly, and each selection must pass the independent invariant checks.
    constexpr int kInstances = 300;
    FixedRandom random(20240601);
    for (int instance = 0; instance < kInstances; ++instance) {
        const std::string name = "random instance " + std::to_string(instance);
        const std::int64_t count = random.between(0, 10);
        std::vector<DeliveryInterval> input;
        for (std::int64_t k = 0; k < count; ++k) {
            const std::int64_t start = random.between(-10, 20);
            const std::int64_t length = random.between(1, 8);
            const std::int64_t profit = random.between(0, 500);
            input.push_back(delivery(std::string(1, static_cast<char>('A' + k)), start, start + length, profit));
        }
        const std::int64_t expected = bruteForceOptimum(input);
        const std::optional<SchedulingResult> result = trySolve(check, name, input);
        if (!result) {
            continue;
        }
        check.expect(result->maximumProfitCents == expected,
                     name + ": optimum " + std::to_string(result->maximumProfitCents) +
                         " differs from brute force " + std::to_string(expected));
        verifyResult(check, name, input, *result);
    }
}

}  // namespace

int main() {
    Checker check;
    testNamedCases(check);
    testPredecessorEdges(check);
    testLongChain(check);
    testOverflow(check);
    testInvalidInput(check);
    testRandomAgainstBruteForce(check);
    return check.report("SchedulingTests") == 0 ? 0 : 1;
}
