#include "pioneer/Scheduling.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

// Weighted interval scheduling (Task A).
//
// Time: O(n log n) for sorting and the binary-search predecessors, then O(n) for the DP
// and traceback. Auxiliary storage: O(n) for the predecessor and DP arrays, plus the
// sorted index order.
//
// Analysis uses 1-based sorted indices. Index 0 is a sentinel so that p(i) = 0 means
// "no compatible earlier delivery" and OPT[0] = 0 needs no special casing.

namespace pioneer {
namespace {

constexpr std::int64_t kMaxCents = std::numeric_limits<std::int64_t>::max();
constexpr std::int64_t kMinCents = std::numeric_limits<std::int64_t>::min();

void requireValidDeliveries(const std::vector<DeliveryInterval>& deliveries) {
    for (const DeliveryInterval& delivery : deliveries) {
        if (delivery.start >= delivery.finish) {
            throw std::invalid_argument("Scheduling: delivery '" + delivery.id + "' must have start < finish");
        }
        if (delivery.profitCents < 0) {
            throw std::invalid_argument("Scheduling: delivery '" + delivery.id + "' has negative profit");
        }
    }
}

// Adds two cent amounts and refuses to wrap around.
std::int64_t checkedAdd(std::int64_t left, std::int64_t right) {
    if ((right > 0 && left > kMaxCents - right) || (right < 0 && left < kMinCents - right)) {
        throw std::overflow_error("Scheduling: total profit exceeds the signed 64-bit cent range");
    }
    return left + right;
}

// Returns the indices of the deliveries sorted by finish time. Ties are broken by start
// time and then by original position, so the order is fully deterministic.
std::vector<std::size_t> sortedOrder(const std::vector<DeliveryInterval>& deliveries) {
    std::vector<std::size_t> order(deliveries.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::sort(order.begin(), order.end(), [&deliveries](std::size_t left, std::size_t right) {
        const DeliveryInterval& a = deliveries[left];
        const DeliveryInterval& b = deliveries[right];
        if (a.finish != b.finish) {
            return a.finish < b.finish;
        }
        if (a.start != b.start) {
            return a.start < b.start;
        }
        return left < right;
    });
    return order;
}

// Computes p(i) for every sorted delivery i in 1..n.
//
// Because finishes are nondecreasing, the entries among finishes[1..i-1] that are <= start[i]
// form a prefix of that range. upper_bound over the prefix returns the count of such entries,
// and that count is the largest such index. The search never reads finishes[i] or later.
std::vector<std::size_t> computePredecessors(const std::vector<std::int64_t>& starts,
                                             const std::vector<std::int64_t>& finishes) {
    const std::size_t n = finishes.size() - 1;
    std::vector<std::size_t> predecessor(n + 1, 0);
    const auto firstFinish = finishes.begin() + 1;
    for (std::size_t i = 1; i <= n; ++i) {
        const auto endOfPrefix = finishes.begin() + static_cast<std::ptrdiff_t>(i);
        const auto afterCompatible = std::upper_bound(firstFinish, endOfPrefix, starts[i]);
        predecessor[i] = static_cast<std::size_t>(afterCompatible - firstFinish);
    }
    return predecessor;
}

}  // namespace

SchedulingResult solveScheduling(const std::vector<DeliveryInterval>& deliveries) {
    requireValidDeliveries(deliveries);

    SchedulingResult result;
    const std::size_t n = deliveries.size();
    if (n == 0) {
        return result;
    }

    const std::vector<std::size_t> order = sortedOrder(deliveries);
    std::vector<std::int64_t> starts(n + 1, 0);
    std::vector<std::int64_t> finishes(n + 1, 0);
    std::vector<std::int64_t> profits(n + 1, 0);
    for (std::size_t i = 1; i <= n; ++i) {
        const DeliveryInterval& delivery = deliveries[order[i - 1]];
        starts[i] = delivery.start;
        finishes[i] = delivery.finish;
        profits[i] = delivery.profitCents;
    }
    const std::vector<std::size_t> predecessor = computePredecessors(starts, finishes);

    // State: best[i] is the maximum profit obtainable from sorted deliveries 1..i.
    // Base case: best[0] = 0 (no deliveries).
    // Recurrence: best[i] = max(best[i-1], profit_i + best[p(i)]). The first term skips
    // delivery i. The second takes it, which leaves only the deliveries 1..p(i) that finish
    // by start_i. Filled in increasing i, so best[p(i)] and best[i-1] are always ready.
    std::vector<std::int64_t> best(n + 1, 0);
    for (std::size_t i = 1; i <= n; ++i) {
        const std::int64_t takeValue = checkedAdd(profits[i], best[predecessor[i]]);
        best[i] = std::max(best[i - 1], takeValue);
    }

    // Traceback from i = n. Tie policy: when best[i] == best[i-1], skipping i reaches the
    // same optimum, so delivery i is excluded. Only a strictly better take includes it.
    std::vector<std::size_t> chosen;
    std::size_t i = n;
    while (i > 0) {
        if (best[i] == best[i - 1]) {
            --i;
        } else {
            chosen.push_back(i);
            i = predecessor[i];
        }
    }
    std::reverse(chosen.begin(), chosen.end());

    // Verify the reconstruction before returning it. Chosen indices come out in finish order,
    // so checking neighbours is enough to show that no two selected deliveries overlap.
    std::int64_t recomputedProfit = 0;
    for (std::size_t k = 0; k < chosen.size(); ++k) {
        const std::size_t current = chosen[k];
        if (k > 0 && finishes[chosen[k - 1]] > starts[current]) {
            throw std::logic_error("Scheduling: reconstructed schedule has overlapping deliveries");
        }
        recomputedProfit = checkedAdd(recomputedProfit, profits[current]);
        result.selectedIds.push_back(deliveries[order[current - 1]].id);
    }
    if (recomputedProfit != best[n]) {
        throw std::logic_error("Scheduling: reconstructed profit does not match the DP optimum");
    }

    result.maximumProfitCents = best[n];
    return result;
}

}  // namespace pioneer
