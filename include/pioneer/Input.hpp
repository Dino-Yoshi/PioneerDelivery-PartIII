#pragma once

#include "pioneer/Cleanup.hpp"
#include "pioneer/Knapsack.hpp"
#include "pioneer/Scheduling.hpp"

#include <cstdint>
#include <iosfwd>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace pioneer {

// Invalid user input. The message names the command, record, or field.
class InputError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

std::vector<DeliveryInterval> parseSchedulingInput(std::istream& in);
KnapsackInput parseKnapsackInput(std::istream& in);
CleanupInput parseCleanupInput(std::istream& in);

// Field helpers, exposed so the numeric and ID rules can be tested directly.
// Accept ASCII decimal digits only (no whitespace, '+', or exponent). allowNegative
// permits one leading '-'. Return nullopt on bad syntax or int64 overflow.
std::optional<std::int64_t> parseDecimalInt64(std::string_view text, bool allowNegative);

// Nonnegative decimal money: digits, optionally '.' followed by one or two digits.
// Returns int64 cents, or nullopt on bad syntax or overflow.
std::optional<std::int64_t> parseCents(std::string_view text);

// IDs match [A-Za-z][A-Za-z0-9_-]*.
bool isValidId(std::string_view text);

}  // namespace pioneer
