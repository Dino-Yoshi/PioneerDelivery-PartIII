#include "pioneer/Output.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace pioneer {
namespace {

std::string joinStrings(const std::vector<std::string>& items, std::string_view separator) {
    std::string text;
    for (std::size_t i = 0; i < items.size(); ++i) {
        if (i > 0) {
            text += separator;
        }
        text += items[i];
    }
    return text;
}

// Human output shows an empty selection as None so it never looks broken.
std::string listOrNone(const std::vector<std::string>& items) {
    return items.empty() ? "None" : joinStrings(items, ", ");
}

std::vector<std::string> indexTexts(const std::vector<std::size_t>& indices) {
    std::vector<std::string> texts;
    texts.reserve(indices.size());
    for (std::size_t index : indices) {
        texts.push_back(std::to_string(index));
    }
    return texts;
}

std::string selectionLine(const std::string& label, const std::vector<std::string>& items) {
    return label + " (" + std::to_string(items.size()) + "): " + listOrNone(items) + "\n";
}

}  // namespace

std::string formatCents(std::int64_t cents) {
    if (cents < 0) {
        throw std::invalid_argument("formatCents: amount is negative");
    }
    std::string text = std::to_string(cents / 100);
    const std::int64_t remainder = cents % 100;
    text += '.';
    if (remainder < 10) {
        text += '0';
    }
    text += std::to_string(remainder);
    return text;
}

std::string renderScheduling(const SchedulingResult& result, bool machine) {
    const std::string profit = formatCents(result.maximumProfitCents);
    if (machine) {
        return "max_profit=" + profit + "\n" +
               "selected_count=" + std::to_string(result.selectedIds.size()) + "\n" +
               "selected_ids=" + joinStrings(result.selectedIds, ",") + "\n";
    }
    return "Weighted interval scheduling\n" +
           std::string("Maximum profit: ") + profit + "\n" +
           selectionLine("Selected deliveries", result.selectedIds);
}

std::string renderKnapsack(const KnapsackResult& result, bool machine) {
    if (machine) {
        return "max_profit=" + std::to_string(result.maximumProfit) + "\n" +
               "total_fuel=" + std::to_string(result.totalFuel) + "\n" +
               "selected_count=" + std::to_string(result.selectedIds.size()) + "\n" +
               "selected_ids=" + joinStrings(result.selectedIds, ",") + "\n";
    }
    return "Delivery selection with a fuel budget\n" +
           std::string("Maximum profit: ") + std::to_string(result.maximumProfit) + "\n" +
           "Total fuel used: " + std::to_string(result.totalFuel) + "\n" +
           selectionLine("Selected deliveries", result.selectedIds);
}

std::string renderCleanup(const CleanupResult& result, bool machine) {
    const std::vector<std::string> removed = indexTexts(result.removedIndices);
    if (machine) {
        return "max_deletions=" + std::to_string(result.maximumDeletions) + "\n" +
               "removed_count=" + std::to_string(removed.size()) + "\n" +
               "removed_indices=" + joinStrings(removed, ",") + "\n";
    }
    return "Event log cleanup\n" +
           std::string("Maximum deletions: ") + std::to_string(result.maximumDeletions) + "\n" +
           selectionLine("Removed original indices", removed);
}

}  // namespace pioneer
