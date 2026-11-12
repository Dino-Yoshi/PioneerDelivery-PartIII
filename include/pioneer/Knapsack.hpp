#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace pioneer {

struct FuelDelivery {
    std::string id;
    std::int64_t fuelCost = 0;  // positive
    std::int64_t profit = 0;    // nonnegative
};

struct KnapsackInput {
    std::int64_t budget = 0;  // nonnegative fuel budget
    std::vector<FuelDelivery> deliveries;
};

struct KnapsackResult {
    std::int64_t maximumProfit = 0;
    std::int64_t totalFuel = 0;
    // Selected IDs in original input order.
    std::vector<std::string> selectedIds;
};

// Precondition: fuel costs are positive, profits and budget are nonnegative, and
// IDs are unique. The input parser enforces these rules.
KnapsackResult solveKnapsack(const KnapsackInput& input);

}  // namespace pioneer
