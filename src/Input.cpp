#include "pioneer/Input.hpp"

#include <cstddef>
#include <cstdint>
#include <istream>
#include <limits>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace pioneer {
namespace {

constexpr std::int64_t kInt64Min = std::numeric_limits<std::int64_t>::min();
constexpr std::int64_t kInt64Max = std::numeric_limits<std::int64_t>::max();

bool isAsciiDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isAsciiLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isLowercaseWord(std::string_view word) {
    if (word.empty()) {
        return false;
    }
    for (char c : word) {
        if (c < 'a' || c > 'z') {
            return false;
        }
    }
    return true;
}

// True when pattern can be matched in order inside source, with gaps allowed.
bool isSubsequence(std::string_view pattern, std::string_view source) {
    std::size_t matched = 0;
    for (char c : source) {
        if (matched < pattern.size() && pattern[matched] == c) {
            ++matched;
        }
    }
    return matched == pattern.size();
}

// Reads the next whitespace-delimited token. The prefix names the record, and
// fieldName names the missing field.
std::string readToken(std::istream& in, const std::string& prefix, const char* fieldName) {
    std::string token;
    if (!(in >> token)) {
        throw InputError(prefix + "missing " + fieldName);
    }
    return token;
}

// Reads an unsigned-style decimal field whose value must be at least minimum.
std::int64_t readIntegerField(std::istream& in, const std::string& prefix, const char* fieldName,
                              std::int64_t minimum) {
    const std::string token = readToken(in, prefix, fieldName);
    const std::optional<std::int64_t> value = parseDecimalInt64(token, false);
    if (!value || *value < minimum) {
        throw InputError(prefix + fieldName + " must be a decimal integer of at least " +
                         std::to_string(minimum) + ", got '" + token + "'");
    }
    return *value;
}

std::int64_t readSignedField(std::istream& in, const std::string& prefix, const char* fieldName) {
    const std::string token = readToken(in, prefix, fieldName);
    const std::optional<std::int64_t> value = parseDecimalInt64(token, true);
    if (!value) {
        throw InputError(prefix + fieldName + " must be a signed 64-bit decimal integer, got '" +
                         token + "'");
    }
    return *value;
}

std::int64_t readCentsField(std::istream& in, const std::string& prefix, const char* fieldName) {
    const std::string token = readToken(in, prefix, fieldName);
    const std::optional<std::int64_t> cents = parseCents(token);
    if (!cents) {
        throw InputError(prefix + fieldName +
                         " must be a nonnegative decimal with at most two fractional digits "
                         "that fits in 64-bit cents, got '" +
                         token + "'");
    }
    return *cents;
}

std::string readId(std::istream& in, const std::string& prefix) {
    std::string id = readToken(in, prefix, "id");
    if (!isValidId(id)) {
        throw InputError(prefix + "invalid id '" + id +
                         "'; ids start with a letter and contain only letters, digits, '_' or '-'");
    }
    return id;
}

void claimId(std::set<std::string>& seenIds, const std::string& id, const std::string& prefix) {
    if (!seenIds.insert(id).second) {
        throw InputError(prefix + "duplicate id '" + id + "'");
    }
}

// Rejects any non-whitespace token left after the expected records.
void requireEndOfInput(std::istream& in, const std::string& command) {
    std::string token;
    if (in >> token) {
        throw InputError(command + "unexpected extra token '" + token +
                         "' after the expected input");
    }
}

// Reads the next nonblank line, which must hold exactly one lowercase word.
// Trailing carriage returns are whitespace, so CRLF files work.
std::string readWordLine(std::istream& in, const std::string& command, const char* fieldName) {
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream words(line);
        std::string word;
        if (!(words >> word)) {
            continue;  // blank line
        }
        std::string extra;
        if (words >> extra) {
            throw InputError(command + fieldName + " line must contain exactly one word");
        }
        if (!isLowercaseWord(word)) {
            throw InputError(command + fieldName +
                             " must contain only lowercase ASCII letters a-z, got '" + word + "'");
        }
        return word;
    }
    throw InputError(command + "missing " + fieldName + " line");
}

}  // namespace

std::vector<DeliveryInterval> parseSchedulingInput(std::istream& in) {
    const std::string command = "schedule: ";
    const std::int64_t count = readIntegerField(in, command, "count n", 0);

    // The count comes from untrusted input, so storage grows with records actually read.
    std::vector<DeliveryInterval> deliveries;
    std::set<std::string> seenIds;
    for (std::int64_t record = 1; record <= count; ++record) {
        const std::string prefix = command + "record " + std::to_string(record) + ": ";
        DeliveryInterval delivery;
        delivery.id = readId(in, prefix);
        claimId(seenIds, delivery.id, prefix);
        delivery.start = readSignedField(in, prefix, "start");
        delivery.finish = readSignedField(in, prefix, "finish");
        if (delivery.start >= delivery.finish) {
            throw InputError(prefix + "finish must be greater than start");
        }
        delivery.profitCents = readCentsField(in, prefix, "profit");
        deliveries.push_back(std::move(delivery));
    }
    requireEndOfInput(in, command);
    return deliveries;
}

KnapsackInput parseKnapsackInput(std::istream& in) {
    const std::string command = "knapsack: ";
    KnapsackInput input;
    const std::int64_t count = readIntegerField(in, command, "count n", 0);
    input.budget = readIntegerField(in, command, "budget B", 0);

    std::set<std::string> seenIds;
    for (std::int64_t record = 1; record <= count; ++record) {
        const std::string prefix = command + "record " + std::to_string(record) + ": ";
        FuelDelivery delivery;
        delivery.id = readId(in, prefix);
        claimId(seenIds, delivery.id, prefix);
        // A zero fuel cost would make the item free, which the Task B model excludes.
        delivery.fuelCost = readIntegerField(in, prefix, "fuel cost", 1);
        delivery.profit = readIntegerField(in, prefix, "profit", 0);
        input.deliveries.push_back(std::move(delivery));
    }
    requireEndOfInput(in, command);
    return input;
}

CleanupInput parseCleanupInput(std::istream& in) {
    const std::string command = "cleanup: ";
    CleanupInput input;
    input.source = readWordLine(in, command, "source");
    input.pattern = readWordLine(in, command, "pattern");
    const std::int64_t count = readIntegerField(in, command, "count k", 0);

    const auto sourceLength = static_cast<std::int64_t>(input.source.size());
    for (std::int64_t i = 0; i < count; ++i) {
        const std::int64_t index = readIntegerField(in, command, "target index", 0);
        if (index >= sourceLength) {
            throw InputError(command + "target index " + std::to_string(index) +
                             " is out of bounds for source length " +
                             std::to_string(input.source.size()));
        }
        const auto position = static_cast<std::size_t>(index);
        // Strictly increasing order also rules out duplicates.
        if (!input.targetIndices.empty() && position <= input.targetIndices.back()) {
            throw InputError(command + "target index " + std::to_string(position) +
                             " must be greater than the previous target index " +
                             std::to_string(input.targetIndices.back()));
        }
        input.targetIndices.push_back(position);
    }
    requireEndOfInput(in, command);

    // Checked here so the algorithm may assume its precondition holds.
    if (!isSubsequence(input.pattern, input.source)) {
        throw InputError(command + "pattern is not a subsequence of source");
    }
    return input;
}

std::optional<std::int64_t> parseDecimalInt64(std::string_view text, bool allowNegative) {
    bool negative = false;
    if (allowNegative && !text.empty() && text.front() == '-') {
        negative = true;
        text.remove_prefix(1);
    }
    if (text.empty()) {
        return std::nullopt;
    }

    // Accumulate as a non-positive value. This makes INT64_MIN representable
    // without ever computing a positive overflow.
    std::int64_t accumulated = 0;
    for (char c : text) {
        if (!isAsciiDigit(c)) {
            return std::nullopt;
        }
        const std::int64_t digit = c - '0';
        // The next value is accumulated * 10 - digit. It must stay at or above kInt64Min.
        if (accumulated < (kInt64Min + digit) / 10) {
            return std::nullopt;
        }
        accumulated = accumulated * 10 - digit;
    }

    if (negative) {
        return accumulated;
    }
    // Magnitude 2^63 is not a positive int64, so reject it.
    if (accumulated == kInt64Min) {
        return std::nullopt;
    }
    return -accumulated;
}

std::optional<std::int64_t> parseCents(std::string_view text) {
    const std::size_t point = text.find('.');
    const std::string_view wholePart = text.substr(0, point);
    if (wholePart.empty()) {
        return std::nullopt;
    }

    std::int64_t fractionCents = 0;
    if (point != std::string_view::npos) {
        const std::string_view fraction = text.substr(point + 1);
        if (fraction.empty() || fraction.size() > 2) {
            return std::nullopt;
        }
        for (char c : fraction) {
            if (!isAsciiDigit(c)) {
                return std::nullopt;
            }
            fractionCents = fractionCents * 10 + (c - '0');
        }
        // A single digit is tenths, so ".5" means 50 cents.
        if (fraction.size() == 1) {
            fractionCents *= 10;
        }
    }

    const std::optional<std::int64_t> dollars = parseDecimalInt64(wholePart, false);
    if (!dollars) {
        return std::nullopt;
    }
    // Check before multiplying by 100 so the cents value cannot overflow.
    if (*dollars > (kInt64Max - fractionCents) / 100) {
        return std::nullopt;
    }
    return *dollars * 100 + fractionCents;
}

bool isValidId(std::string_view text) {
    if (text.empty() || !isAsciiLetter(text.front())) {
        return false;
    }
    for (char c : text) {
        const bool allowed = isAsciiLetter(c) || isAsciiDigit(c) || c == '_' || c == '-';
        if (!allowed) {
            return false;
        }
    }
    return true;
}

}  // namespace pioneer
