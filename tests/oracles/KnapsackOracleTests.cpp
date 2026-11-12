// Independent oracle tests for Task B (0/1 knapsack with a fuel budget).
//
// The oracle enumerates every inclusion mask over the deliveries. It discards masks whose fuel total
// exceeds the budget and keeps the largest profit among the rest. It uses no DP table, traceback, or
// production helper, so it shares no code path with the production solver.
//
// Phase 1 checks the oracle against the expected values in docs/11_TEST_PLAN.md and the assignment.
// Phase 2 compares the production solver with the oracle and checks the reconstruction invariants of
// the production result with code written here.

#include "TestHarness.hpp"

#include "pioneer/Knapsack.hpp"

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

using pioneer::FuelDelivery;
using pioneer::KnapsackInput;
using pioneer::KnapsackResult;
using pioneer::testing::Checker;

constexpr std::size_t kMaxOracleDeliveries = 10;
constexpr std::size_t kMaxRandomDeliveries = 9;
constexpr std::uint64_t kFirstSeed = 1;
constexpr std::uint64_t kLastSeed = 25;
constexpr std::uint64_t kRandomMaxBudget = 30;
constexpr std::uint64_t kRandomMaxFuelCost = 12;
constexpr std::uint64_t kRandomMaxProfit = 50;

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

FuelDelivery makeFuelDelivery(const std::string& id, std::int64_t fuelCost, std::int64_t profit) {
    FuelDelivery delivery;
    delivery.id = id;
    delivery.fuelCost = fuelCost;
    delivery.profit = profit;
    return delivery;
}

KnapsackInput makeInput(std::int64_t budget, std::vector<FuelDelivery> deliveries) {
    KnapsackInput input;
    input.budget = budget;
    input.deliveries = std::move(deliveries);
    return input;
}

struct NamedCase {
    std::string caseId;
    KnapsackInput input;
    std::int64_t expectedProfit;
};

// Named cases B-01..B-08 from docs/11_TEST_PLAN.md.
std::vector<NamedCase> namedKnapsackCases() {
    return {
        // B-01: the example from the assignment. Q, R, S uses 10 fuel for profit 53.
        {"B-01",
         makeInput(10, {makeFuelDelivery("P", 4, 20), makeFuelDelivery("Q", 3, 14),
                        makeFuelDelivery("R", 5, 29), makeFuelDelivery("S", 2, 10),
                        makeFuelDelivery("T", 6, 31)}),
         53},
        // B-02: no deliveries.
        {"B-02", makeInput(10, {}), 0},
        // B-03: zero budget, and every delivery needs fuel.
        {"B-03", makeInput(0, {makeFuelDelivery("A", 2, 5), makeFuelDelivery("B", 3, 7)}), 0},
        // B-04: A fits exactly (4 fuel, profit 20). B alone needs 5 fuel and does not fit.
        {"B-04", makeInput(4, {makeFuelDelivery("A", 4, 20), makeFuelDelivery("B", 5, 21)}), 20},
        // B-05: a profit-per-fuel greedy takes P (30). The optimum is Q + R (48).
        {"B-05",
         makeInput(10, {makeFuelDelivery("P", 6, 30), makeFuelDelivery("Q", 5, 24),
                        makeFuelDelivery("R", 5, 24)}),
         48},
        // B-06: equal profit at a fuel tie. Either delivery alone gives 5.
        {"B-06", makeInput(3, {makeFuelDelivery("A", 2, 5), makeFuelDelivery("B", 3, 5)}), 5},
        // B-07: every delivery costs more than the budget.
        {"B-07", makeInput(4, {makeFuelDelivery("A", 5, 10), makeFuelDelivery("B", 7, 20)}), 0},
        // B-08: A (3 fuel, profit 9) beats B (2 fuel, profit 6). A and B together need 5 fuel, so
        // they cannot both be taken, and 1 fuel stays unused.
        {"B-08", makeInput(4, {makeFuelDelivery("A", 3, 9), makeFuelDelivery("B", 2, 6)}), 9},
    };
}

std::string describeInput(const KnapsackInput& input) {
    std::ostringstream text;
    text << "budget=" << input.budget << " n=" << input.deliveries.size();
    for (const FuelDelivery& delivery : input.deliveries) {
        text << "; " << delivery.id << " fuel=" << delivery.fuelCost
             << " profit=" << delivery.profit;
    }
    return text.str();
}

// Returns the largest profit among inclusion masks whose fuel total stays within the budget.
std::int64_t oracleOptimumProfit(const KnapsackInput& input) {
    const std::size_t count = input.deliveries.size();
    if (count > kMaxOracleDeliveries) {
        throw std::invalid_argument("knapsack oracle is limited to 10 deliveries");
    }
    std::int64_t best = 0;
    const std::uint32_t subsetCount = std::uint32_t{1} << count;
    for (std::uint32_t mask = 0; mask < subsetCount; ++mask) {
        std::int64_t fuel = 0;
        std::int64_t profit = 0;
        for (std::size_t i = 0; i < count; ++i) {
            if (((mask >> i) & 1u) != 0) {
                fuel += input.deliveries[i].fuelCost;
                profit += input.deliveries[i].profit;
            }
        }
        if (fuel <= input.budget) {
            best = std::max(best, profit);
        }
    }
    return best;
}

// Returns the position of the delivery with this ID in the input, or -1 when it is absent.
std::int64_t findPosition(const std::vector<FuelDelivery>& deliveries, const std::string& id) {
    for (std::size_t i = 0; i < deliveries.size(); ++i) {
        if (deliveries[i].id == id) {
            return static_cast<std::int64_t>(i);
        }
    }
    return -1;
}

// Checks the reconstruction invariants of a production result. Returns an empty string when all
// invariants hold, otherwise a description of the first violation.
std::string knapsackViolation(const KnapsackInput& input, const KnapsackResult& result) {
    std::int64_t fuelSum = 0;
    std::int64_t profitSum = 0;
    // Positions strictly increase along the selection, which enforces input order and rejects duplicates.
    std::int64_t previousPosition = -1;
    for (const std::string& id : result.selectedIds) {
        const std::int64_t position = findPosition(input.deliveries, id);
        if (position < 0) {
            return "selected ID '" + id + "' is not in the input";
        }
        if (position <= previousPosition) {
            return "selected IDs are not in original input order, or one appears twice";
        }
        previousPosition = position;
        const FuelDelivery& record = input.deliveries[static_cast<std::size_t>(position)];
        fuelSum += record.fuelCost;
        profitSum += record.profit;
    }

    if (result.totalFuel != fuelSum) {
        return "reported total fuel " + std::to_string(result.totalFuel) +
               " differs from the selected cost sum " + std::to_string(fuelSum);
    }
    if (fuelSum > input.budget) {
        return "selected fuel " + std::to_string(fuelSum) + " exceeds the budget " +
               std::to_string(input.budget);
    }
    if (result.maximumProfit != profitSum) {
        return "profit of the selected deliveries " + std::to_string(profitSum) +
               " differs from reported maximum " + std::to_string(result.maximumProfit);
    }
    return "";
}

KnapsackInput randomKnapsackInput(LinearCongruentialGenerator& rng) {
    KnapsackInput input;
    input.budget = static_cast<std::int64_t>(rng.nextBelow(kRandomMaxBudget + 1));
    const std::size_t count = static_cast<std::size_t>(rng.nextBelow(kMaxRandomDeliveries + 1));
    for (std::size_t i = 0; i < count; ++i) {
        // Fuel costs are at least 1, as the assignment requires.
        const std::int64_t fuelCost = 1 + static_cast<std::int64_t>(rng.nextBelow(kRandomMaxFuelCost));
        const std::int64_t profit = static_cast<std::int64_t>(rng.nextBelow(kRandomMaxProfit + 1));
        const std::string id(1, static_cast<char>('A' + i));
        input.deliveries.push_back(makeFuelDelivery(id, fuelCost, profit));
    }
    return input;
}

void runOracleSelfChecks(Checker& check) {
    for (const NamedCase& namedCase : namedKnapsackCases()) {
        const std::int64_t oracle = oracleOptimumProfit(namedCase.input);
        check.expect(oracle == namedCase.expectedProfit,
                     namedCase.caseId + ": oracle optimum " + std::to_string(oracle) +
                         " differs from expected " + std::to_string(namedCase.expectedProfit));
    }
}

void checkProductionAgainstOracle(Checker& check, const std::string& label,
                                  const KnapsackInput& input) {
    const std::int64_t oracle = oracleOptimumProfit(input);
    const KnapsackResult result = pioneer::solveKnapsack(input);
    const std::string instance = describeInput(input);

    check.expect(result.maximumProfit == oracle,
                 label + ": production optimum " + std::to_string(result.maximumProfit) +
                     " differs from oracle " + std::to_string(oracle) + "; instance: " + instance);
    const std::string violation = knapsackViolation(input, result);
    check.expect(violation.empty(), label + ": " + violation + "; instance: " + instance);
}

// Phase 2 does not catch exceptions. A stub that throws std::logic_error stops this phase, and main
// reports the message as a failed check.
void runProductionComparison(Checker& check) {
    for (const NamedCase& namedCase : namedKnapsackCases()) {
        checkProductionAgainstOracle(check, namedCase.caseId, namedCase.input);
    }
    for (std::uint64_t seed = kFirstSeed; seed <= kLastSeed; ++seed) {
        LinearCongruentialGenerator rng(seed);
        checkProductionAgainstOracle(check, "seed " + std::to_string(seed), randomKnapsackInput(rng));
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
