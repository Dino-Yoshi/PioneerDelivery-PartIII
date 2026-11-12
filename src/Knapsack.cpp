#include "pioneer/Knapsack.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace pioneer {

namespace {

// Largest DP table this solver allocates: 256 MiB. Tables above this size are refused.
// The solver never falls back to a different table layout to get under the limit.
constexpr std::size_t kMaxTableBytes = 256 * 1024 * 1024;

constexpr std::int64_t kInt64Max = std::numeric_limits<std::int64_t>::max();
constexpr std::int64_t kInt64Min = std::numeric_limits<std::int64_t>::min();

// Returns a + b for signed 64-bit values. Throws std::overflow_error when the sum does not fit.
std::int64_t checkedAdd(std::int64_t a, std::int64_t b) {
    if (b > 0 && a > kInt64Max - b) {
        throw std::overflow_error("knapsack profit sum overflows signed 64-bit integer");
    }
    if (b < 0 && a < kInt64Min - b) {
        throw std::overflow_error("knapsack profit sum overflows signed 64-bit integer");
    }
    return a + b;
}

// Flat storage index of cell K[row][column] in a table with the given number of columns.
std::size_t tableIndex(std::size_t row, std::size_t column, std::size_t columns) {
    return row * columns + column;
}

// Checks the table dimensions (n+1) rows by (B+1) columns before anything is allocated.
// Returns the number of cells. Throws std::length_error when the size arithmetic overflows
// or when the table would exceed kMaxTableBytes (8 bytes per cell).
std::size_t checkedTableCells(std::size_t deliveryCount, std::int64_t budget) {
    const std::size_t maxSize = std::numeric_limits<std::size_t>::max();
    if (budget == kInt64Max || static_cast<std::uint64_t>(budget) >= maxSize) {
        throw std::length_error("knapsack table: budget " + std::to_string(budget) +
                                " overflows the size type when (B+1) is computed");
    }
    if (deliveryCount == maxSize) {
        throw std::length_error("knapsack table: delivery count makes (n+1) overflow the size type");
    }
    const std::size_t rows = deliveryCount + 1;
    const std::size_t columns = static_cast<std::size_t>(budget) + 1;

    if (columns > maxSize / rows) {
        throw std::length_error("knapsack table: (n+1)=" + std::to_string(rows) + " by (B+1)=" +
                                std::to_string(columns) + " overflows the size type");
    }
    const std::size_t cells = rows * columns;
    const std::size_t maxCells = kMaxTableBytes / sizeof(std::int64_t);
    if (cells > maxCells) {
        throw std::length_error("knapsack table: (n+1)=" + std::to_string(rows) + " by (B+1)=" +
                                std::to_string(columns) + " is " + std::to_string(cells) +
                                " cells, which exceeds the limit of " +
                                std::to_string(kMaxTableBytes) + " bytes (" +
                                std::to_string(maxCells) + " cells)");
    }
    return cells;
}

}  // namespace

KnapsackResult solveKnapsack(const KnapsackInput& input) {
    if (input.budget < 0) {
        throw std::invalid_argument("knapsack budget must be nonnegative");
    }
    const std::vector<FuelDelivery>& items = input.deliveries;
    for (const FuelDelivery& item : items) {
        if (item.fuelCost < 1) {
            throw std::invalid_argument("knapsack delivery '" + item.id + "' must have fuel cost >= 1");
        }
        if (item.profit < 0) {
            throw std::invalid_argument("knapsack delivery '" + item.id + "' must have profit >= 0");
        }
    }

    KnapsackResult result;
    // With no deliveries the only answer is the empty selection, so no table is needed.
    if (items.empty()) {
        return result;
    }

    const std::size_t deliveryCount = items.size();
    const std::size_t cells = checkedTableCells(deliveryCount, input.budget);
    const std::size_t rows = deliveryCount + 1;
    const std::size_t budget = static_cast<std::size_t>(input.budget);
    const std::size_t columns = budget + 1;

    // Row i holds K[i][0..B]. Every cell starts at 0, which supplies both base cases:
    // K[0][b] = 0 (no deliveries) and K[i][0] = 0 (costs are at least 1, so nothing fits).
    std::vector<std::int64_t> table(cells, 0);

    // Bottom-up fill, row by row. K[i][b] is the best profit from the first i deliveries
    // with fuel at most b. For delivery i (1-based) with cost c and profit v:
    //   c > b:  K[i][b] = K[i-1][b]                          (delivery i cannot be afforded)
    //   c <= b: K[i][b] = max(K[i-1][b], v + K[i-1][b - c])  (skip i, or take i once)
    // Both reads use row i-1, so no delivery is counted twice.
    for (std::size_t i = 1; i < rows; ++i) {
        const FuelDelivery& item = items[i - 1];
        for (std::size_t b = 0; b <= budget; ++b) {
            const std::int64_t skipValue = table[tableIndex(i - 1, b, columns)];
            std::int64_t best = skipValue;
            if (item.fuelCost <= static_cast<std::int64_t>(b)) {
                const std::size_t spare = b - static_cast<std::size_t>(item.fuelCost);
                const std::int64_t takeValue =
                    checkedAdd(item.profit, table[tableIndex(i - 1, spare, columns)]);
                if (takeValue > best) {
                    best = takeValue;
                }
            }
            table[tableIndex(i, b, columns)] = best;
        }
    }

    // Traceback from (n, B). Ties exclude: when K[i][b] equals K[i-1][b], skipping i already
    // reaches the optimum, so i is left out. Otherwise K[i][b] must come from taking i, which
    // means its cost fits, so mark i and continue from (i-1, b - cost_i).
    std::vector<bool> chosen(deliveryCount, false);
    std::size_t remainingBudget = budget;
    for (std::size_t i = deliveryCount; i > 0; --i) {
        const std::int64_t current = table[tableIndex(i, remainingBudget, columns)];
        if (current == table[tableIndex(i - 1, remainingBudget, columns)]) {
            continue;
        }
        const FuelDelivery& item = items[i - 1];
        const std::size_t cost = static_cast<std::size_t>(item.fuelCost);
        if (cost > remainingBudget) {
            throw std::logic_error("knapsack reconstruction took an unaffordable delivery");
        }
        chosen[i - 1] = true;
        remainingBudget -= cost;
    }

    // Collect the chosen IDs in input order and recompute fuel and profit from the items.
    std::set<std::string> seenIds;
    std::int64_t profitSum = 0;
    for (std::size_t i = 0; i < deliveryCount; ++i) {
        if (!chosen[i]) {
            continue;
        }
        const FuelDelivery& item = items[i];
        if (!seenIds.insert(item.id).second) {
            throw std::logic_error("knapsack reconstruction selected duplicate id '" + item.id + "'");
        }
        result.totalFuel = checkedAdd(result.totalFuel, item.fuelCost);
        profitSum = checkedAdd(profitSum, item.profit);
        result.selectedIds.push_back(item.id);
    }

    result.maximumProfit = table[tableIndex(deliveryCount, budget, columns)];
    if (result.totalFuel > input.budget) {
        throw std::logic_error("knapsack reconstruction exceeds the fuel budget");
    }
    if (profitSum != result.maximumProfit) {
        throw std::logic_error("knapsack reconstruction profit " + std::to_string(profitSum) +
                               " does not match table optimum " +
                               std::to_string(result.maximumProfit));
    }
    return result;
}

}  // namespace pioneer
