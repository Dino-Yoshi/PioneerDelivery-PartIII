// Formatter tests: exact human and --machine output for given results.
// No algorithm is called, so these tests need no DP answers.

#include "TestHarness.hpp"

#include "pioneer/Output.hpp"

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using pioneer::testing::Checker;

constexpr std::int64_t kInt64Max = std::numeric_limits<std::int64_t>::max();

void testFormatCents(Checker& check) {
    check.expect(pioneer::formatCents(0) == "0.00", "cents: zero prints as 0.00");
    check.expect(pioneer::formatCents(1) == "0.01", "cents: one cent prints as 0.01");
    check.expect(pioneer::formatCents(10) == "0.10", "cents: ten cents keeps the zero");
    check.expect(pioneer::formatCents(5050) == "50.50", "cents: 50.50");
    check.expect(pioneer::formatCents(18000) == "180.00", "cents: 180.00");
    check.expect(pioneer::formatCents(kInt64Max) == "92233720368547758.07",
                 "cents: INT64_MAX prints exactly");

    bool threw = false;
    try {
        pioneer::formatCents(-1);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    check.expect(threw, "cents: negative amount throws");
}

void testSchedulingOutput(Checker& check) {
    pioneer::SchedulingResult result;
    result.maximumProfitCents = 18000;
    result.selectedIds = {"B", "D", "F"};

    check.expect(pioneer::renderScheduling(result, true) ==
                     "max_profit=180.00\nselected_count=3\nselected_ids=B,D,F\n",
                 "schedule machine output has exact keys and order");
    check.expect(pioneer::renderScheduling(result, false) ==
                     "Weighted interval scheduling\n"
                     "Maximum profit: 180.00\n"
                     "Selected deliveries (3): B, D, F\n",
                 "schedule human output");

    pioneer::SchedulingResult empty;
    check.expect(pioneer::renderScheduling(empty, true) ==
                     "max_profit=0.00\nselected_count=0\nselected_ids=\n",
                 "schedule machine output for an empty selection");
    check.expect(pioneer::renderScheduling(empty, false) ==
                     "Weighted interval scheduling\n"
                     "Maximum profit: 0.00\n"
                     "Selected deliveries (0): None\n",
                 "schedule human output for an empty selection says None");
}

void testKnapsackOutput(Checker& check) {
    pioneer::KnapsackResult result;
    result.maximumProfit = 53;
    result.totalFuel = 10;
    result.selectedIds = {"Q", "R", "S"};

    check.expect(pioneer::renderKnapsack(result, true) ==
                     "max_profit=53\ntotal_fuel=10\nselected_count=3\nselected_ids=Q,R,S\n",
                 "knapsack machine output has exact keys and order");
    check.expect(pioneer::renderKnapsack(result, false) ==
                     "Delivery selection with a fuel budget\n"
                     "Maximum profit: 53\n"
                     "Total fuel used: 10\n"
                     "Selected deliveries (3): Q, R, S\n",
                 "knapsack human output");

    pioneer::KnapsackResult empty;
    check.expect(pioneer::renderKnapsack(empty, true) ==
                     "max_profit=0\ntotal_fuel=0\nselected_count=0\nselected_ids=\n",
                 "knapsack machine output for an empty selection");
    check.expect(pioneer::renderKnapsack(empty, false) ==
                     "Delivery selection with a fuel budget\n"
                     "Maximum profit: 0\n"
                     "Total fuel used: 0\n"
                     "Selected deliveries (0): None\n",
                 "knapsack human output for an empty selection says None");
}

void testCleanupOutput(Checker& check) {
    pioneer::CleanupResult result;
    result.maximumDeletions = 1;
    result.removedIndices = {2};

    check.expect(pioneer::renderCleanup(result, true) ==
                     "max_deletions=1\nremoved_count=1\nremoved_indices=2\n",
                 "cleanup machine output has exact keys and order");
    check.expect(pioneer::renderCleanup(result, false) ==
                     "Event log cleanup\n"
                     "Maximum deletions: 1\n"
                     "Removed original indices (1): 2\n",
                 "cleanup human output");

    pioneer::CleanupResult multiple;
    multiple.maximumDeletions = 2;
    multiple.removedIndices = {2, 3};
    check.expect(pioneer::renderCleanup(multiple, true) ==
                     "max_deletions=2\nremoved_count=2\nremoved_indices=2,3\n",
                 "cleanup machine output joins indices with commas and no spaces");

    pioneer::CleanupResult empty;
    check.expect(pioneer::renderCleanup(empty, true) ==
                     "max_deletions=0\nremoved_count=0\nremoved_indices=\n",
                 "cleanup machine output for no deletions");
    check.expect(pioneer::renderCleanup(empty, false) ==
                     "Event log cleanup\n"
                     "Maximum deletions: 0\n"
                     "Removed original indices (0): None\n",
                 "cleanup human output for no deletions says None");
}

}  // namespace

int main() {
    Checker check;
    testFormatCents(check);
    testSchedulingOutput(check);
    testKnapsackOutput(check);
    testCleanupOutput(check);
    return check.report("formatter_tests") == 0 ? 0 : 1;
}
