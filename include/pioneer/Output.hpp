#pragma once

#include "pioneer/Cleanup.hpp"
#include "pioneer/Knapsack.hpp"
#include "pioneer/Scheduling.hpp"

#include <cstdint>
#include <string>

namespace pioneer {

// Formats nonnegative cents as "D.CC" with exactly two fractional digits.
// Throws std::invalid_argument for negative values.
std::string formatCents(std::int64_t cents);

// Each render function formats an already computed result. It does no DP.
// machine=false gives the readable report; machine=true gives docs/05 key=value
// lines. Both forms end with a newline.
std::string renderScheduling(const SchedulingResult& result, bool machine);
std::string renderKnapsack(const KnapsackResult& result, bool machine);
std::string renderCleanup(const CleanupResult& result, bool machine);

}  // namespace pioneer
