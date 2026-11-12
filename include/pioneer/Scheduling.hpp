#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace pioneer {

// One delivery request for Task A. Times are signed 64-bit integers and profit
// is exact int64 cents, so no floating-point value enters the DP.
struct DeliveryInterval {
    std::string id;
    std::int64_t start = 0;
    std::int64_t finish = 0;
    std::int64_t profitCents = 0;
};

struct SchedulingResult {
    std::int64_t maximumProfitCents = 0;
    // Selected IDs in chronological (finish-time) order.
    std::vector<std::string> selectedIds;
};

// Precondition: every interval has start < finish and profitCents >= 0, and IDs
// are unique. The input parser enforces these rules.
SchedulingResult solveScheduling(const std::vector<DeliveryInterval>& deliveries);

}  // namespace pioneer
