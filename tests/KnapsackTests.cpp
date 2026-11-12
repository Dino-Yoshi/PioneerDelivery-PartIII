// Knapsack tests: the named B cases, table-limit and overflow refusals, and a fixed-seed
// comparison against subset enumeration. Every successful result is also checked by code in
// this file that does not use the solver's reconstruction.

#include "TestHarness.hpp"

#include "pioneer/Knapsack.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
#include <map>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using pioneer::FuelDelivery;
using pioneer::KnapsackInput;
using pioneer::KnapsackResult;
using pioneer::solveKnapsack;
using pioneer::testing::Checker;

constexpr std::int64_t kInt64Max = std::numeric_limits<std::int64_t>::max();

// Seed for the randomized comparison. It is printed in every failure description.
constexpr std::uint64_t kRandomSeed = 20240607;

FuelDelivery makeItem(const std::string& id, std::int64_t fuelCost, std::int64_t profit) {
    FuelDelivery item;
    item.id = id;
    item.fuelCost = fuelCost;
    item.profit = profit;
    return item;
}

// Optimum by enumerating every subset. It shares no code with the solver's table or traceback.
std::int64_t bruteForceOptimum(const KnapsackInput& input) {
    const std::size_t n = input.deliveries.size();
    std::int64_t best = 0;
    for (std::uint32_t mask = 0; mask < (std::uint32_t{1} << n); ++mask) {
        std::int64_t fuel = 0;
        std::int64_t profit = 0;
        for (std::size_t i = 0; i < n; ++i) {
            if ((mask >> i) & 1U) {
                fuel += input.deliveries[i].fuelCost;
                profit += input.deliveries[i].profit;
            }
        }
        if (fuel <= input.budget && profit > best) {
            best = profit;
        }
    }
    return best;
}

// Checks the validity invariants of a result using only the input and the returned values.
void verifyResult(Checker& check, const KnapsackInput& input, const KnapsackResult& result,
                  const std::string& label) {
    std::map<std::string, std::size_t> positionById;
    for (std::size_t i = 0; i < input.deliveries.size(); ++i) {
        positionById[input.deliveries[i].id] = i;
    }

    bool allKnown = true;
    bool allUnique = true;
    bool inInputOrder = true;
    std::set<std::string> seen;
    std::int64_t fuel = 0;
    std::int64_t profit = 0;
    bool hasPrevious = false;
    std::size_t previousPosition = 0;

    for (const std::string& id : result.selectedIds) {
        const auto found = positionById.find(id);
        if (found == positionById.end()) {
            allKnown = false;
            continue;
        }
        if (!seen.insert(id).second) {
            allUnique = false;
        }
        if (hasPrevious && found->second <= previousPosition) {
            inInputOrder = false;
        }
        hasPrevious = true;
        previousPosition = found->second;
        fuel += input.deliveries[found->second].fuelCost;
        profit += input.deliveries[found->second].profit;
    }

    check.expect(allKnown, label + ": every selected id appears in the input");
    check.expect(allUnique, label + ": no id is selected twice");
    check.expect(inInputOrder, label + ": selected ids are in original input order");
    check.expect(fuel <= input.budget, label + ": summed fuel is within the budget");
    check.expect(result.totalFuel == fuel,
                 label + ": reported totalFuel equals the sum of selected costs");
    check.expect(profit == result.maximumProfit,
                 label + ": selected profit equals the reported maximum");
}

// Runs the solver on a named case and checks the optimum exactly. An unexpected exception
// is reported as a failed check so that the remaining cases still run.
void checkNamedCase(Checker& check, const std::string& name, const KnapsackInput& input,
                    std::int64_t expectedOptimum) {
    try {
        const KnapsackResult result = solveKnapsack(input);
        check.expect(result.maximumProfit == expectedOptimum,
                     name + ": optimum " + std::to_string(result.maximumProfit) +
                         ", expected " + std::to_string(expectedOptimum));
        check.expect(bruteForceOptimum(input) == expectedOptimum,
                     name + ": expected value agrees with subset enumeration");
        verifyResult(check, input, result, name);
    } catch (const std::exception& error) {
        check.expect(false, name + ": unexpected exception: " + error.what());
    }
}

void testNamedCases(Checker& check) {
    KnapsackInput officialExample;
    officialExample.budget = 10;
    officialExample.deliveries = {makeItem("P", 4, 20), makeItem("Q", 3, 14),
                                  makeItem("R", 5, 29), makeItem("S", 2, 10),
                                  makeItem("T", 6, 31)};
    checkNamedCase(check, "B-01 official example", officialExample, 53);

    KnapsackInput noItems;
    noItems.budget = 10;
    checkNamedCase(check, "B-02 no items", noItems, 0);
    try {
        const KnapsackResult result = solveKnapsack(noItems);
        check.expect(result.selectedIds.empty() && result.totalFuel == 0,
                     "B-02 no items: empty selection and zero fuel");
    } catch (const std::exception& error) {
        check.expect(false, std::string("B-02 no items: unexpected exception: ") + error.what());
    }

    KnapsackInput zeroBudget;
    zeroBudget.budget = 0;
    zeroBudget.deliveries = {makeItem("A", 1, 5), makeItem("B", 2, 7)};
    checkNamedCase(check, "B-03 zero budget", zeroBudget, 0);

    KnapsackInput exactFit;
    exactFit.budget = 4;
    exactFit.deliveries = {makeItem("A", 4, 20), makeItem("B", 5, 21)};
    checkNamedCase(check, "B-04 exact fit", exactFit, 20);

    KnapsackInput ratioCounterexample;
    ratioCounterexample.budget = 10;
    ratioCounterexample.deliveries = {makeItem("P", 6, 30), makeItem("Q", 5, 24),
                                      makeItem("R", 5, 24)};
    checkNamedCase(check, "B-05 ratio greedy counterexample", ratioCounterexample, 48);

    KnapsackInput profitTie;
    profitTie.budget = 3;
    profitTie.deliveries = {makeItem("A", 2, 5), makeItem("B", 3, 5)};
    checkNamedCase(check, "B-06 profit tie", profitTie, 5);

    KnapsackInput allTooExpensive;
    allTooExpensive.budget = 3;
    allTooExpensive.deliveries = {makeItem("A", 4, 100), makeItem("B", 7, 50)};
    checkNamedCase(check, "B-07 every item too expensive", allTooExpensive, 0);
    try {
        const KnapsackResult result = solveKnapsack(allTooExpensive);
        check.expect(result.selectedIds.empty(), "B-07 every item too expensive: empty selection");
    } catch (const std::exception& error) {
        check.expect(false, std::string("B-07: unexpected exception: ") + error.what());
    }

    KnapsackInput unusedBudget;
    unusedBudget.budget = 4;
    unusedBudget.deliveries = {makeItem("A", 3, 9), makeItem("B", 2, 6)};
    checkNamedCase(check, "B-08 budget left unused", unusedBudget, 9);
}

void testInvalidArguments(Checker& check) {
    KnapsackInput negativeBudget;
    negativeBudget.budget = -1;
    negativeBudget.deliveries = {makeItem("A", 1, 1)};
    check.expectThrows<std::invalid_argument>([&] { solveKnapsack(negativeBudget); },
                                              "budget must be nonnegative",
                                              "negative budget is rejected");

    KnapsackInput zeroCost;
    zeroCost.budget = 5;
    zeroCost.deliveries = {makeItem("A", 0, 1)};
    check.expectThrows<std::invalid_argument>([&] { solveKnapsack(zeroCost); },
                                              "fuel cost >= 1",
                                              "zero fuel cost is rejected");

    KnapsackInput negativeProfit;
    negativeProfit.budget = 5;
    negativeProfit.deliveries = {makeItem("A", 1, -1)};
    check.expectThrows<std::invalid_argument>([&] { solveKnapsack(negativeProfit); },
                                              "profit >= 0",
                                              "negative profit is rejected");
}

// The table holds (n+1) rows of (B+1) 8-byte cells. With n = 1 that is 2 rows, so
// 256 MiB allows (B+1) <= 16777216 and the largest accepted budget is 16777215.
void testTableLimit(Checker& check) {
    KnapsackInput overLimit;
    overLimit.budget = 16777216;
    overLimit.deliveries = {makeItem("A", 1, 5)};
    check.expectThrows<std::length_error>([&] { solveKnapsack(overLimit); },
                                          "exceeds the limit of 268435456 bytes",
                                          "budget 16777216 with n=1 exceeds the table limit");

    KnapsackInput atLimit;
    atLimit.budget = 16777215;
    atLimit.deliveries = {makeItem("A", 1, 5)};
    try {
        const auto start = std::chrono::steady_clock::now();
        const KnapsackResult result = solveKnapsack(atLimit);
        const auto elapsed = std::chrono::steady_clock::now() - start;
        std::cout << "at-limit table test (budget 16777215, n=1, 268435456 bytes): "
                  << std::chrono::duration<double>(elapsed).count() << " s\n";
        check.expect(result.maximumProfit == 5, "budget 16777215 with n=1 succeeds with optimum 5");
        verifyResult(check, atLimit, result, "at-limit table test");
    } catch (const std::exception& error) {
        check.expect(false, std::string("budget 16777215 with n=1 must succeed: ") + error.what());
    }
}

void testSizeArithmeticOverflow(Checker& check) {
    KnapsackInput maxBudget;
    maxBudget.budget = kInt64Max;
    maxBudget.deliveries = {makeItem("A", 1, 5)};
    check.expectThrows<std::length_error>([&] { solveKnapsack(maxBudget); }, "overflows",
                                          "budget INT64_MAX with n=1 overflows (B+1)");

    KnapsackInput nearMaxBudget;
    nearMaxBudget.budget = kInt64Max - 1;
    nearMaxBudget.deliveries = {makeItem("A", 1, 5), makeItem("B", 2, 6), makeItem("C", 3, 7)};
    check.expectThrows<std::length_error>([&] { solveKnapsack(nearMaxBudget); }, "overflows",
                                          "budget INT64_MAX-1 with n=3 overflows (n+1)*(B+1)");
}

void testProfitOverflow(Checker& check) {
    KnapsackInput overflowing;
    overflowing.budget = 2;
    overflowing.deliveries = {makeItem("A", 1, kInt64Max), makeItem("B", 1, kInt64Max)};
    check.expectThrows<std::overflow_error>([&] { solveKnapsack(overflowing); }, "overflows",
                                            "profits summing above INT64_MAX throw overflow_error");
}

void testZeroDeliveriesHugeBudget(Checker& check) {
    KnapsackInput empty;
    empty.budget = kInt64Max;
    try {
        const KnapsackResult result = solveKnapsack(empty);
        check.expect(result.maximumProfit == 0 && result.totalFuel == 0 &&
                         result.selectedIds.empty(),
                     "n=0 with budget INT64_MAX returns the empty result");
    } catch (const std::exception& error) {
        check.expect(false, std::string("n=0 with huge budget must not throw: ") + error.what());
    }
}

// Compares the solver with subset enumeration on fixed-seed random instances.
void testRandomInstances(Checker& check) {
    std::mt19937_64 generator(kRandomSeed);
    std::uniform_int_distribution<int> countDistribution(0, 8);
    std::uniform_int_distribution<int> costDistribution(1, 8);
    std::uniform_int_distribution<int> profitDistribution(0, 20);
    std::uniform_int_distribution<int> budgetDistribution(0, 30);

    constexpr int kCases = 300;
    for (int caseIndex = 0; caseIndex < kCases; ++caseIndex) {
        KnapsackInput input;
        input.budget = budgetDistribution(generator);
        const int count = countDistribution(generator);
        for (int i = 0; i < count; ++i) {
            input.deliveries.push_back(makeItem(std::string(1, static_cast<char>('a' + i)),
                                                costDistribution(generator),
                                                profitDistribution(generator)));
        }
        const std::string label = "random case " + std::to_string(caseIndex) + " (seed " +
                                  std::to_string(kRandomSeed) + ")";
        try {
            const KnapsackResult result = solveKnapsack(input);
            const std::int64_t expected = bruteForceOptimum(input);
            check.expect(result.maximumProfit == expected,
                         label + ": optimum " + std::to_string(result.maximumProfit) +
                             ", expected " + std::to_string(expected));
            verifyResult(check, input, result, label);
        } catch (const std::exception& error) {
            check.expect(false, label + ": unexpected exception: " + error.what());
        }
    }
}

}  // namespace

int main() {
    Checker check;
    testNamedCases(check);
    testInvalidArguments(check);
    testTableLimit(check);
    testSizeArithmeticOverflow(check);
    testProfitOverflow(check);
    testZeroDeliveriesHugeBudget(check);
    testRandomInstances(check);
    return check.report("knapsack_tests") == 0 ? 0 : 1;
}
