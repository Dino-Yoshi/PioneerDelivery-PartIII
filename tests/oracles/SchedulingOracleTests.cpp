// Independent oracle tests for Task A (weighted interval scheduling).
//
// The oracle enumerates every subset of the deliveries and keeps the largest profit among subsets
// whose members are pairwise compatible. It uses no sorted-DP table, predecessor array, or
// traceback, so it shares no code path with the production solver.
//
// Phase 1 checks the oracle against the expected values in docs/11_TEST_PLAN.md and the assignment.
// Phase 2 compares the production solver with the oracle and checks the reconstruction invariants of
// the production result with code written here.

#include "TestHarness.hpp"

#include "pioneer/Scheduling.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using pioneer::DeliveryInterval;
using pioneer::SchedulingResult;
using pioneer::testing::Checker;

constexpr std::size_t kMaxOracleDeliveries = 10;
constexpr std::size_t kMaxRandomDeliveries = 8;
constexpr std::uint64_t kFirstSeed = 1;
constexpr std::uint64_t kLastSeed = 25;
constexpr std::int64_t kRandomStartSpan = 30;    // starts fall in [-10, 20)
constexpr std::int64_t kRandomStartOffset = 10;
constexpr std::uint64_t kRandomMaxDuration = 8;  // durations fall in [1, 8]
constexpr std::uint64_t kRandomMaxProfitCents = 5000;

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

DeliveryInterval makeDelivery(const std::string& id, std::int64_t start, std::int64_t finish,
                              std::int64_t profitCents) {
    DeliveryInterval delivery;
    delivery.id = id;
    delivery.start = start;
    delivery.finish = finish;
    delivery.profitCents = profitCents;
    return delivery;
}

struct NamedCase {
    std::string caseId;
    std::vector<DeliveryInterval> deliveries;
    std::int64_t expectedProfitCents;
};

// Named cases A-01..A-08 from docs/11_TEST_PLAN.md. Profits are in cents.
std::vector<NamedCase> namedSchedulingCases() {
    return {
        // A-01: the example from the assignment. Optimum B, D, F = 60 + 70 + 50 = 180.00.
        {"A-01",
         {makeDelivery("A", 0, 3, 5000), makeDelivery("B", 1, 4, 6000), makeDelivery("C", 3, 5, 4000),
          makeDelivery("D", 4, 7, 7000), makeDelivery("E", 5, 8, 5000), makeDelivery("F", 7, 9, 5000)},
         18000},
        // A-02: no deliveries.
        {"A-02", {}, 0},
        // A-03: finish equal to the next start is compatible. 30.00 + 40.00 = 70.00.
        {"A-03", {makeDelivery("A", 0, 2, 3000), makeDelivery("B", 2, 5, 4000)}, 7000},
        // A-04: every pair overlaps, so only the most profitable delivery can be kept.
        {"A-04",
         {makeDelivery("A", 0, 3, 5000), makeDelivery("B", 1, 4, 6000), makeDelivery("C", 2, 5, 4000)},
         6000},
        // A-05: two identical intervals. Only one can be selected.
        {"A-05", {makeDelivery("A", 0, 2, 5000), makeDelivery("B", 0, 2, 5000)}, 5000},
        // A-06: earliest-finish greedy would pick A and C (20.00). The optimum is B alone (100.00).
        {"A-06",
         {makeDelivery("A", 0, 2, 1000), makeDelivery("B", 0, 3, 10000), makeDelivery("C", 2, 4, 1000)},
         10000},
        // A-07: every profit is zero, so the optimum is zero.
        {"A-07",
         {makeDelivery("A", 0, 2, 0), makeDelivery("B", 2, 4, 0), makeDelivery("C", 4, 6, 0)},
         0},
        // A-08: negative times, equal finish values (Q and R both finish at -2), shuffled input order.
        // Optima: Q+S = 25 + 30 and P+R+S = 10 + 15 + 30, both 55.00.
        {"A-08",
         {makeDelivery("S", -2, 0, 3000), makeDelivery("Q", -4, -2, 2500), makeDelivery("T", -1, 1, 2000),
          makeDelivery("P", -6, -3, 1000), makeDelivery("R", -3, -2, 1500)},
         5500},
    };
}

std::string describeDeliveries(const std::vector<DeliveryInterval>& deliveries) {
    std::ostringstream text;
    text << "n=" << deliveries.size();
    for (const DeliveryInterval& delivery : deliveries) {
        text << "; " << delivery.id << " [" << delivery.start << "," << delivery.finish
             << ") profitCents=" << delivery.profitCents;
    }
    return text.str();
}

// Enumerates subsets by bit mask: bit i selects delivery i.
std::vector<std::size_t> indicesInMask(std::uint32_t mask, std::size_t count) {
    std::vector<std::size_t> indices;
    for (std::size_t i = 0; i < count; ++i) {
        if (((mask >> i) & 1u) != 0) {
            indices.push_back(i);
        }
    }
    return indices;
}

// Sorts a subset by start, then finish, and requires each finish to be no later than the next start.
// For positive-length intervals this neighbour test is equivalent to pairwise compatibility, because
// finish(j) <= start(j+1) < finish(j+1) <= start(j+2) chains through the sorted order.
bool isCompatibleSubset(const std::vector<DeliveryInterval>& deliveries,
                        std::vector<std::size_t> chosen) {
    std::sort(chosen.begin(), chosen.end(), [&deliveries](std::size_t a, std::size_t b) {
        if (deliveries[a].start != deliveries[b].start) {
            return deliveries[a].start < deliveries[b].start;
        }
        return deliveries[a].finish < deliveries[b].finish;
    });
    for (std::size_t i = 1; i < chosen.size(); ++i) {
        if (deliveries[chosen[i - 1]].finish > deliveries[chosen[i]].start) {
            return false;
        }
    }
    return true;
}

// Returns the largest total profit (in cents) of a pairwise compatible subset.
std::int64_t oracleOptimumCents(const std::vector<DeliveryInterval>& deliveries) {
    const std::size_t count = deliveries.size();
    if (count > kMaxOracleDeliveries) {
        throw std::invalid_argument("scheduling oracle is limited to 10 deliveries");
    }
    std::int64_t best = 0;
    const std::uint32_t subsetCount = std::uint32_t{1} << count;
    for (std::uint32_t mask = 0; mask < subsetCount; ++mask) {
        const std::vector<std::size_t> chosen = indicesInMask(mask, count);
        if (!isCompatibleSubset(deliveries, chosen)) {
            continue;
        }
        std::int64_t profit = 0;
        for (const std::size_t index : chosen) {
            profit += deliveries[index].profitCents;
        }
        best = std::max(best, profit);
    }
    return best;
}

const DeliveryInterval* findRecord(const std::vector<DeliveryInterval>& deliveries,
                                   const std::string& id) {
    for (const DeliveryInterval& record : deliveries) {
        if (record.id == id) {
            return &record;
        }
    }
    return nullptr;
}

// Checks the reconstruction invariants of a production result. Returns an empty string when all
// invariants hold, otherwise a description of the first violation.
std::string schedulingViolation(const std::vector<DeliveryInterval>& deliveries,
                                const SchedulingResult& result) {
    std::vector<const DeliveryInterval*> chosen;
    for (const std::string& id : result.selectedIds) {
        const DeliveryInterval* record = findRecord(deliveries, id);
        if (record == nullptr) {
            return "selected ID '" + id + "' is not in the input";
        }
        for (const DeliveryInterval* earlier : chosen) {
            if (earlier->id == id) {
                return "selected ID '" + id + "' appears twice";
            }
        }
        chosen.push_back(record);
    }

    // Chronological order means nondecreasing finish time.
    for (std::size_t i = 1; i < chosen.size(); ++i) {
        if (chosen[i]->finish < chosen[i - 1]->finish) {
            return "selected IDs are not in chronological (finish-time) order";
        }
    }

    // In chronological order, an earlier delivery must finish no later than a later delivery starts.
    std::int64_t profitSum = 0;
    for (std::size_t a = 0; a < chosen.size(); ++a) {
        profitSum += chosen[a]->profitCents;
        for (std::size_t b = a + 1; b < chosen.size(); ++b) {
            if (chosen[a]->finish > chosen[b]->start) {
                return "selected deliveries '" + chosen[a]->id + "' and '" + chosen[b]->id +
                       "' overlap";
            }
        }
    }

    if (profitSum != result.maximumProfitCents) {
        return "profit of the selected deliveries " + std::to_string(profitSum) +
               " differs from reported maximum " + std::to_string(result.maximumProfitCents);
    }
    return "";
}

DeliveryInterval randomDelivery(LinearCongruentialGenerator& rng, std::size_t index) {
    const std::int64_t start =
        static_cast<std::int64_t>(rng.nextBelow(kRandomStartSpan)) - kRandomStartOffset;
    const std::int64_t duration = 1 + static_cast<std::int64_t>(rng.nextBelow(kRandomMaxDuration));
    const std::int64_t profit = static_cast<std::int64_t>(rng.nextBelow(kRandomMaxProfitCents + 1));
    const std::string id(1, static_cast<char>('A' + index));
    return makeDelivery(id, start, start + duration, profit);
}

std::vector<DeliveryInterval> randomDeliveries(LinearCongruentialGenerator& rng) {
    const std::size_t count = static_cast<std::size_t>(rng.nextBelow(kMaxRandomDeliveries + 1));
    std::vector<DeliveryInterval> deliveries;
    for (std::size_t i = 0; i < count; ++i) {
        deliveries.push_back(randomDelivery(rng, i));
    }
    return deliveries;
}

void runOracleSelfChecks(Checker& check) {
    for (const NamedCase& namedCase : namedSchedulingCases()) {
        const std::int64_t oracle = oracleOptimumCents(namedCase.deliveries);
        check.expect(oracle == namedCase.expectedProfitCents,
                     namedCase.caseId + ": oracle optimum " + std::to_string(oracle) +
                         " cents differs from expected " +
                         std::to_string(namedCase.expectedProfitCents) + " cents");
    }
}

void checkProductionAgainstOracle(Checker& check, const std::string& label,
                                  const std::vector<DeliveryInterval>& deliveries) {
    const std::int64_t oracle = oracleOptimumCents(deliveries);
    const SchedulingResult result = pioneer::solveScheduling(deliveries);
    const std::string instance = describeDeliveries(deliveries);

    check.expect(result.maximumProfitCents == oracle,
                 label + ": production optimum " + std::to_string(result.maximumProfitCents) +
                     " cents differs from oracle " + std::to_string(oracle) +
                     " cents; instance: " + instance);
    const std::string violation = schedulingViolation(deliveries, result);
    check.expect(violation.empty(), label + ": " + violation + "; instance: " + instance);
}

// Phase 2 does not catch exceptions. A stub that throws std::logic_error stops this phase, and main
// reports the message as a failed check.
void runProductionComparison(Checker& check) {
    for (const NamedCase& namedCase : namedSchedulingCases()) {
        checkProductionAgainstOracle(check, namedCase.caseId, namedCase.deliveries);
    }
    for (std::uint64_t seed = kFirstSeed; seed <= kLastSeed; ++seed) {
        LinearCongruentialGenerator rng(seed);
        checkProductionAgainstOracle(check, "seed " + std::to_string(seed), randomDeliveries(rng));
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
