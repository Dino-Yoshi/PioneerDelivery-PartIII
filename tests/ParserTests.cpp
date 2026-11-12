// Parser tests: numeric and ID rules, plus the three input grammars.
// These call only the parsers, so they do not depend on any algorithm result.

#include "TestHarness.hpp"

#include "pioneer/Input.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace {

using pioneer::CleanupInput;
using pioneer::DeliveryInterval;
using pioneer::InputError;
using pioneer::KnapsackInput;
using pioneer::testing::Checker;

constexpr std::int64_t kInt64Max = std::numeric_limits<std::int64_t>::max();
constexpr std::int64_t kInt64Min = std::numeric_limits<std::int64_t>::min();

bool holds(const std::optional<std::int64_t>& actual, std::int64_t expected) {
    return actual.has_value() && *actual == expected;
}

// Runs a parser and reports whether it accepted the input.
template <typename Function>
bool accepts(Function function) {
    try {
        function();
        return true;
    } catch (const InputError&) {
        return false;
    }
}

std::vector<DeliveryInterval> scheduleInput(const std::string& text) {
    std::istringstream in(text);
    return pioneer::parseSchedulingInput(in);
}

KnapsackInput knapsackInput(const std::string& text) {
    std::istringstream in(text);
    return pioneer::parseKnapsackInput(in);
}

CleanupInput cleanupInput(const std::string& text) {
    std::istringstream in(text);
    return pioneer::parseCleanupInput(in);
}

void testIntegerParsing(Checker& check) {
    using pioneer::parseDecimalInt64;
    check.expect(holds(parseDecimalInt64("0", false), 0), "integer: zero is accepted");
    check.expect(holds(parseDecimalInt64("007", false), 7), "integer: leading zeros are decimal");
    check.expect(holds(parseDecimalInt64("9223372036854775807", false), kInt64Max),
                 "integer: INT64_MAX is accepted");
    check.expect(!parseDecimalInt64("9223372036854775808", false),
                 "integer: INT64_MAX + 1 is rejected");
    check.expect(!parseDecimalInt64("99999999999999999999", false),
                 "integer: far overflow is rejected");
    check.expect(holds(parseDecimalInt64("-9223372036854775808", true), kInt64Min),
                 "integer: INT64_MIN is accepted when signed");
    check.expect(!parseDecimalInt64("-9223372036854775809", true),
                 "integer: below INT64_MIN is rejected");
    check.expect(holds(parseDecimalInt64("-42", true), -42),
                 "integer: negative value accepted when signed");
    check.expect(!parseDecimalInt64("-1", false), "integer: sign rejected for unsigned fields");
    check.expect(!parseDecimalInt64("", false), "integer: empty text is rejected");
    check.expect(!parseDecimalInt64("-", true), "integer: lone sign is rejected");
    check.expect(!parseDecimalInt64("+1", true), "integer: plus sign is rejected");
    check.expect(!parseDecimalInt64(" 1", false), "integer: leading whitespace is rejected");
    check.expect(!parseDecimalInt64("1.0", false), "integer: fraction is rejected");
    check.expect(!parseDecimalInt64("1e3", false), "integer: exponent is rejected");
    check.expect(!parseDecimalInt64("0x10", false), "integer: hexadecimal is rejected");
}

void testCentsParsing(Checker& check) {
    using pioneer::parseCents;
    check.expect(holds(parseCents("0"), 0), "cents: zero");
    check.expect(holds(parseCents("50"), 5000), "cents: whole number");
    check.expect(holds(parseCents("50.0"), 5000), "cents: one fractional digit");
    check.expect(holds(parseCents("50.00"), 5000), "cents: two fractional digits");
    check.expect(holds(parseCents("0.1"), 10), "cents: 0.1 is ten cents");
    check.expect(holds(parseCents("0.10"), 10), "cents: 0.10 is ten cents");
    check.expect(holds(parseCents("0.01"), 1), "cents: smallest unit");
    check.expect(holds(parseCents("92233720368547758.07"), kInt64Max),
                 "cents: INT64_MAX cents is accepted");
    check.expect(!parseCents("92233720368547758.08"), "cents: one cent above INT64_MAX is rejected");
    check.expect(!parseCents("92233720368547759"), "cents: whole part overflow is rejected");
    check.expect(!parseCents("50."), "cents: trailing point is rejected");
    check.expect(!parseCents(".5"), "cents: missing integer digits are rejected");
    check.expect(!parseCents("50.000"), "cents: three fractional digits are rejected");
    check.expect(!parseCents("1.2.3"), "cents: two points are rejected");
    check.expect(!parseCents("1.-5"), "cents: sign in fraction is rejected");
    check.expect(!parseCents("-1"), "cents: negative is rejected");
    check.expect(!parseCents("+1"), "cents: plus sign is rejected");
    check.expect(!parseCents("1,50"), "cents: comma decimal separator is rejected");
    check.expect(!parseCents("1e2"), "cents: exponent is rejected");
    check.expect(!parseCents(""), "cents: empty text is rejected");
}

void testIds(Checker& check) {
    using pioneer::isValidId;
    check.expect(isValidId("A"), "id: single letter");
    check.expect(isValidId("a1"), "id: letter then digit");
    check.expect(isValidId("B_2-x"), "id: underscore and hyphen are allowed");
    check.expect(!isValidId(""), "id: empty is rejected");
    check.expect(!isValidId("1A"), "id: leading digit is rejected");
    check.expect(!isValidId("_A"), "id: leading underscore is rejected");
    check.expect(!isValidId("-A"), "id: leading hyphen is rejected");
    check.expect(!isValidId("A.B"), "id: period is rejected");
    check.expect(!isValidId("A B"), "id: whitespace is rejected");
    check.expect(!isValidId("A\xC3\xA9"), "id: non-ASCII letter is rejected");
}

void testSchedulingInput(Checker& check) {
    const std::vector<DeliveryInterval> example = scheduleInput(
        "6\nA 0 3 50.00\nB 1 4 60.00\nC 3 5 40.00\nD 4 7 70.00\nE 5 8 50.00\nF 7 9 50.00\n");
    check.expect(example.size() == 6, "schedule: official example has six records");
    if (example.size() == 6) {
        check.expect(example[1].id == "B" && example[1].start == 1 && example[1].finish == 4 &&
                         example[1].profitCents == 6000,
                     "schedule: record B keeps its id, times, and profit in cents");
        check.expect(example[5].profitCents == 5000, "schedule: profit 50.00 is 5000 cents");
    }

    check.expect(scheduleInput("0").empty(), "schedule: exactly 0 is the empty set");
    check.expect(scheduleInput("0 \n\n").empty(),
                 "schedule: 0 followed by whitespace is the empty set");
    check.expect(scheduleInput("2\nA 0 3 1\nB 3 4 1\n").size() == 2,
                 "schedule: touching endpoints are accepted");
    const std::vector<DeliveryInterval> negative = scheduleInput("1\nA -5 -1 0\n");
    check.expect(negative.size() == 1 && negative[0].start == -5 && negative[0].finish == -1 &&
                     negative[0].profitCents == 0,
                 "schedule: negative times and zero profit are accepted");
    check.expect(scheduleInput("\n1\n\nA 0 1 2\n\n").size() == 1,
                 "schedule: blank lines between tokens are ignored");
    check.expect(accepts([] { scheduleInput("1\nA 0 9223372036854775807 1\n"); }),
                 "schedule: INT64_MAX finish is accepted");

    check.expectThrows<InputError>([] { scheduleInput("1\nA 3 3 10\n"); },
                                   "record 1: finish must be greater than start",
                                   "schedule: equal start and finish is rejected");
    check.expectThrows<InputError>([] { scheduleInput("2\nA 0 3 1\nB 5 2 1\n"); },
                                   "record 2: finish must be greater than start",
                                   "schedule: finish before start names record 2");
    check.expectThrows<InputError>([] { scheduleInput("2\nA 0 3 1\nA 4 5 1\n"); },
                                   "record 2: duplicate id 'A'",
                                   "schedule: duplicate id is rejected");
    check.expectThrows<InputError>([] { scheduleInput("1\n1A 0 3 5\n"); },
                                   "record 1: invalid id '1A'",
                                   "schedule: invalid id is rejected");
    check.expectThrows<InputError>([] { scheduleInput("1\nA 0 3 1.001\n"); },
                                   "record 1: profit",
                                   "schedule: three fractional digits are rejected");
    check.expectThrows<InputError>([] { scheduleInput("1\nA 0 3 -1\n"); },
                                   "record 1: profit",
                                   "schedule: negative profit is rejected");
    check.expectThrows<InputError>([] { scheduleInput("1\nA 9223372036854775808 9223372036854775809 1\n"); },
                                   "record 1: start must be a signed 64-bit",
                                   "schedule: start overflow is rejected");
    check.expectThrows<InputError>([] { scheduleInput("1\nA 0 9223372036854775808 1\n"); },
                                   "record 1: finish must be a signed 64-bit",
                                   "schedule: finish overflow is rejected");
    check.expectThrows<InputError>([] { scheduleInput("1\nA 0 3\n"); },
                                   "record 1: missing profit",
                                   "schedule: missing profit field is rejected");
    check.expectThrows<InputError>([] { scheduleInput("2\nA 0 3 1\n"); },
                                   "record 2: missing id",
                                   "schedule: fewer records than n is rejected");
    check.expectThrows<InputError>([] { scheduleInput("0 extra"); },
                                   "unexpected extra token 'extra'",
                                   "schedule: extra token after empty set is rejected");
    check.expectThrows<InputError>([] { scheduleInput("1\nA 0 3 1\nB 0 1 1\n"); },
                                   "unexpected extra token 'B'",
                                   "schedule: extra record beyond n is rejected");
    check.expectThrows<InputError>([] { scheduleInput("x\n"); },
                                   "count n must be a decimal integer",
                                   "schedule: non-numeric count is rejected");
    check.expectThrows<InputError>([] { scheduleInput("-1\n"); },
                                   "count n must be a decimal integer",
                                   "schedule: negative count is rejected");
    check.expectThrows<InputError>([] { scheduleInput("1.5\n"); },
                                   "count n must be a decimal integer",
                                   "schedule: fractional count is rejected");
    check.expectThrows<InputError>([] { scheduleInput(""); },
                                   "missing count n",
                                   "schedule: empty input is rejected");
}

void testKnapsackInput(Checker& check) {
    const KnapsackInput example = knapsackInput("5 10\nP 4 20\nQ 3 14\nR 5 29\nS 2 10\nT 6 31\n");
    check.expect(example.budget == 10 && example.deliveries.size() == 5,
                 "knapsack: official example has budget 10 and five deliveries");
    if (example.deliveries.size() == 5) {
        check.expect(example.deliveries[0].id == "P" && example.deliveries[0].fuelCost == 4 &&
                         example.deliveries[0].profit == 20,
                     "knapsack: record P keeps fuel cost and profit");
    }

    const KnapsackInput empty = knapsackInput("0 0\n");
    check.expect(empty.budget == 0 && empty.deliveries.empty(),
                 "knapsack: zero deliveries and zero budget");
    check.expect(accepts([] { knapsackInput("1 0\nA 1 0\n"); }),
                 "knapsack: zero budget with an item that does not fit is accepted");
    check.expect(accepts([] { knapsackInput("1 5\nA 5 1\n"); }),
                 "knapsack: item that exactly fits the budget is accepted");
    check.expect(accepts([] { knapsackInput("1 5\nA 1 0\n"); }),
                 "knapsack: zero profit is accepted");

    check.expectThrows<InputError>([] { knapsackInput("1 5\nA 0 3\n"); },
                                   "record 1: fuel cost must be a decimal integer of at least 1",
                                   "knapsack: zero fuel cost is rejected");
    check.expectThrows<InputError>([] { knapsackInput("1 5\nA -1 3\n"); },
                                   "record 1: fuel cost",
                                   "knapsack: negative fuel cost is rejected");
    check.expectThrows<InputError>([] { knapsackInput("1 5\nA 1 -3\n"); },
                                   "record 1: profit",
                                   "knapsack: negative profit is rejected");
    check.expectThrows<InputError>([] { knapsackInput("1 5\nA 99999999999999999999 1\n"); },
                                   "record 1: fuel cost",
                                   "knapsack: fuel cost overflow is rejected");
    check.expectThrows<InputError>([] { knapsackInput("1 -5\nA 1 1\n"); },
                                   "budget B",
                                   "knapsack: negative budget is rejected");
    check.expectThrows<InputError>([] { knapsackInput("x 5\n"); },
                                   "count n",
                                   "knapsack: non-numeric count is rejected");
    check.expectThrows<InputError>([] { knapsackInput("1\n"); },
                                   "missing budget B",
                                   "knapsack: missing budget is rejected");
    check.expectThrows<InputError>([] { knapsackInput("2 5\nA 1 1\nA 2 2\n"); },
                                   "record 2: duplicate id 'A'",
                                   "knapsack: duplicate id is rejected");
    check.expectThrows<InputError>([] { knapsackInput("1 5\nA 1\n"); },
                                   "record 1: missing profit",
                                   "knapsack: missing profit is rejected");
    check.expectThrows<InputError>([] { knapsackInput("1 5\nA 1 1\nextra\n"); },
                                   "unexpected extra token 'extra'",
                                   "knapsack: extra token is rejected");
}

void testCleanupInput(Checker& check) {
    const CleanupInput example = cleanupInput("abbaa\naba\n3\n0 1 2\n");
    check.expect(example.source == "abbaa" && example.pattern == "aba",
                 "cleanup: official example strings");
    check.expect(example.targetIndices == std::vector<std::size_t>({0, 1, 2}),
                 "cleanup: official example indices");

    const CleanupInput noTargets = cleanupInput("bcda\nd\n0");
    check.expect(noTargets.targetIndices.empty(),
                 "cleanup: k=0 needs no index line and accepts no trailing newline");
    check.expect(accepts([] { cleanupInput("\n\nabbaa\n\naba\n3\n0 1 2\n"); }),
                 "cleanup: blank lines before and between the words are ignored");
    check.expect(accepts([] { cleanupInput("abbaa\naba\n3\n0\n1 2\n"); }),
                 "cleanup: indices may span lines");
    check.expect(accepts([] { cleanupInput("abbaa\r\naba\r\n3\r\n0 1 2\r\n"); }),
                 "cleanup: CRLF line endings are accepted");
    check.expect(accepts([] { cleanupInput("abbaa\naba\n1\n4\n"); }),
                 "cleanup: last valid index (length - 1) is accepted");
    check.expect(accepts([] { cleanupInput("abbaa\naba\n1\n0\n"); }),
                 "cleanup: index 0 is accepted");

    check.expectThrows<InputError>([] { cleanupInput("Abc\nb\n0\n"); },
                                   "source must contain only lowercase",
                                   "cleanup: uppercase source is rejected");
    check.expectThrows<InputError>([] { cleanupInput("ab1\nb\n0\n"); },
                                   "source must contain only lowercase",
                                   "cleanup: digit in source is rejected");
    check.expectThrows<InputError>([] { cleanupInput("ab cd\nb\n0\n"); },
                                   "source line must contain exactly one word",
                                   "cleanup: two words on the source line are rejected");
    check.expectThrows<InputError>([] { cleanupInput("abc\n\n"); },
                                   "missing pattern line",
                                   "cleanup: missing pattern line is rejected");
    check.expectThrows<InputError>([] { cleanupInput("abc\nb\n"); },
                                   "missing count k",
                                   "cleanup: missing count is rejected");
    check.expectThrows<InputError>([] { cleanupInput("abc\nb\n-1\n"); },
                                   "count k must be a decimal integer",
                                   "cleanup: negative count is rejected");
    check.expectThrows<InputError>([] { cleanupInput("abc\nb\n1\n"); },
                                   "missing target index",
                                   "cleanup: missing index is rejected");
    check.expectThrows<InputError>([] { cleanupInput("abbaa\naba\n1\n7\n"); },
                                   "target index 7 is out of bounds for source length 5",
                                   "cleanup: out of bounds index is rejected");
    check.expectThrows<InputError>([] { cleanupInput("abbaa\naba\n1\n5\n"); },
                                   "target index 5 is out of bounds for source length 5",
                                   "cleanup: index equal to source length is rejected");
    check.expectThrows<InputError>([] { cleanupInput("abbaa\naba\n1\n-1\n"); },
                                   "got '-1'",
                                   "cleanup: negative index is rejected");
    check.expectThrows<InputError>([] { cleanupInput("abbaa\naba\n2\n1 1\n"); },
                                   "must be greater than the previous target index",
                                   "cleanup: duplicate index is rejected");
    check.expectThrows<InputError>([] { cleanupInput("abbaa\naba\n2\n2 1\n"); },
                                   "must be greater than the previous target index",
                                   "cleanup: decreasing indices are rejected");
    check.expectThrows<InputError>([] { cleanupInput("abbaa\naba\n1\n1 2\n"); },
                                   "unexpected extra token '2'",
                                   "cleanup: extra index beyond k is rejected");
    check.expectThrows<InputError>([] { cleanupInput("abbaa\naba\n0\n3\n"); },
                                   "unexpected extra token '3'",
                                   "cleanup: index after k=0 is rejected");
    check.expectThrows<InputError>([] { cleanupInput("abc\nacb\n0\n"); },
                                   "pattern is not a subsequence of source",
                                   "cleanup: pattern not a subsequence is rejected");
    check.expectThrows<InputError>([] { cleanupInput("ab\nabc\n0\n"); },
                                   "pattern is not a subsequence of source",
                                   "cleanup: pattern longer than source is rejected");
}

}  // namespace

int main() {
    Checker check;
    testIntegerParsing(check);
    testCentsParsing(check);
    testIds(check);
    testSchedulingInput(check);
    testKnapsackInput(check);
    testCleanupInput(check);
    return check.report("parser_tests") == 0 ? 0 : 1;
}
