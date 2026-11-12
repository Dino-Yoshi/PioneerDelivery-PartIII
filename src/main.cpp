// Command-line dispatch for the pioneer executable. This file handles options,
// help, input reading, and exit status. Parsing, algorithms, and rendering live
// in their own modules, and no DP happens here.

#include "pioneer/Cleanup.hpp"
#include "pioneer/Input.hpp"
#include "pioneer/Knapsack.hpp"
#include "pioneer/Output.hpp"
#include "pioneer/Scheduling.hpp"

#include <cstddef>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace {

// Exit statuses from docs/05_CLI_CONTRACT.md.
constexpr int kExitOk = 0;
constexpr int kExitOperational = 1;
constexpr int kExitUsage = 2;

enum class Task { Schedule, Knapsack, Cleanup };

constexpr std::string_view kRootHelp = R"HELP(pioneer: dynamic programming tools for delivery planning

Usage:
  pioneer <command> [--input PATH] [--machine]
  pioneer --help

Commands:
  schedule   Maximum-profit set of non-overlapping deliveries (weighted interval scheduling)
  knapsack   Maximum-profit deliveries within a fuel budget (0/1 knapsack)
  cleanup    Maximum event-log deletions that keep a pattern as a subsequence
  Run 'pioneer <command> --help' for the input format and an example.

Options:
  --input PATH   Read the problem from PATH. Without it, the problem is read from stdin.
  --machine      Print stable key=value lines instead of the readable report.
  -h, --help     Show this help and exit.

Example:
  pioneer schedule --input examples/schedule.txt

Exit status:
  0  success or help
  1  operational failure, such as an unreadable input file
  2  usage error or invalid input
)HELP";

constexpr std::string_view kScheduleHelp = R"HELP(Usage: pioneer schedule [--input PATH] [--machine]

Chooses non-overlapping deliveries with the largest total profit. A delivery that
finishes at time t does not conflict with one that starts at time t.

Input (whitespace-separated; blank lines are ignored):
  n
  id start finish profit      repeated n times

  id      starts with a letter, then letters, digits, '_' or '-'; unique in the input
  start   signed 64-bit integer, must be less than finish
  finish  signed 64-bit integer
  profit  nonnegative decimal with at most two fractional digits, such as 50, 50.5, or 0.01

Example input:
  3
  A 0 3 50.00
  B 1 4 60.00
  C 3 5 40.00

Options:
  --input PATH   Read the input from PATH instead of standard input
  --machine      Print max_profit, selected_count, and selected_ids as key=value lines
  -h, --help     Show this help and exit

Exit status: 0 success or help, 1 operational failure, 2 usage or invalid input
)HELP";

constexpr std::string_view kKnapsackHelp = R"HELP(Usage: pioneer knapsack [--input PATH] [--machine]

Chooses each delivery at most once so that total fuel stays within the budget and total
profit is as large as possible.

Input (whitespace-separated; blank lines are ignored):
  n B
  id fuelCost profit      repeated n times

  n, B      nonnegative integers: n is the number of deliveries, B is the fuel budget
  id        starts with a letter, then letters, digits, '_' or '-'; unique in the input
  fuelCost  positive integer
  profit    nonnegative integer

Example input:
  5 10
  P 4 20
  Q 3 14
  R 5 29
  S 2 10
  T 6 31

Options:
  --input PATH   Read the input from PATH instead of standard input
  --machine      Print max_profit, total_fuel, selected_count, and selected_ids as key=value lines
  -h, --help     Show this help and exit

Exit status: 0 success or help, 1 operational failure, 2 usage or invalid input
)HELP";

constexpr std::string_view kCleanupHelp = R"HELP(Usage: pioneer cleanup [--input PATH] [--machine]

Deletes as many characters as possible from source, using only the listed original
positions, while pattern stays a subsequence of source. Deleting a character does not
shift the positions of the others.

Input (blank lines before the first two words are ignored):
  source                   nonempty lowercase word on its own line
  pattern                  nonempty lowercase word on its own line, a subsequence of source
  k                        nonnegative count of target indices
  index0 ... index(k-1)    k strictly increasing zero-based positions; may span lines

Example input:
  abbaa
  aba
  3
  0 1 2

Options:
  --input PATH   Read the input from PATH instead of standard input
  --machine      Print max_deletions, removed_count, and removed_indices as key=value lines
  -h, --help     Show this help and exit

Exit status: 0 success or help, 1 operational failure, 2 usage or invalid input
)HELP";

std::string_view taskName(Task task) {
    switch (task) {
        case Task::Schedule:
            return "schedule";
        case Task::Knapsack:
            return "knapsack";
        case Task::Cleanup:
            return "cleanup";
    }
    return "";
}

std::string_view helpText(Task task) {
    switch (task) {
        case Task::Schedule:
            return kScheduleHelp;
        case Task::Knapsack:
            return kKnapsackHelp;
        case Task::Cleanup:
            return kCleanupHelp;
    }
    return kRootHelp;
}

std::optional<Task> findTask(std::string_view name) {
    if (name == "schedule") {
        return Task::Schedule;
    }
    if (name == "knapsack") {
        return Task::Knapsack;
    }
    if (name == "cleanup") {
        return Task::Cleanup;
    }
    return std::nullopt;
}

std::string taskUsage(Task task) {
    const std::string name(taskName(task));
    return "usage: pioneer " + name + " [--input PATH] [--machine]\n" +
           "Run 'pioneer " + name + " --help' for details.\n";
}

// Reports a usage error on stderr and returns the usage exit status.
int reportUsage(const std::string& message, const std::string& usage) {
    std::cerr << message << '\n' << usage;
    return kExitUsage;
}

// Reads the whole input before any parsing, so no partial output can appear.
// Returns false after reporting the problem when the file cannot be read.
bool readInputText(const std::optional<std::string>& inputPath, std::string& text) {
    std::ostringstream buffer;
    if (inputPath) {
        std::error_code ec;
        const bool isDirectory = std::filesystem::is_directory(*inputPath, ec);
        std::ifstream file(*inputPath, std::ios::binary);
        if (isDirectory || !file) {
            std::cerr << "pioneer: cannot read input file '" << *inputPath << "'\n";
            return false;
        }
        buffer << file.rdbuf();
    } else {
        buffer << std::cin.rdbuf();
    }
    text = buffer.str();
    return true;
}

// Normalizes text that came from a file written on Windows. A UTF-8 byte order mark, which
// some editors add, is dropped. A UTF-16 file (the default output of some PowerShell
// redirections) cannot be parsed, so it is named in the error instead of surfacing as a
// confusing "invalid token". Returns an error message, or an empty string when the text
// is usable.
std::string prepareInputText(std::string& text) {
    constexpr std::string_view kUtf8Bom = "\xEF\xBB\xBF";
    if (text.size() >= kUtf8Bom.size() && text.compare(0, kUtf8Bom.size(), kUtf8Bom) == 0) {
        text.erase(0, kUtf8Bom.size());
    }
    if (text.size() >= 2) {
        const auto first = static_cast<unsigned char>(text[0]);
        const auto second = static_cast<unsigned char>(text[1]);
        if ((first == 0xFF && second == 0xFE) || (first == 0xFE && second == 0xFF)) {
            return "the input looks like UTF-16 text. Save it as UTF-8 or ASCII and run again "
                   "(in Windows PowerShell, use 'Out-File -Encoding ascii' instead of '>')";
        }
    }
    return std::string();
}

// Parses, solves, and renders one task. Throws pioneer::InputError for invalid
// input and std::exception for operational failures.
std::string runTask(Task task, bool machine, const std::string& text) {
    std::istringstream in(text);
    switch (task) {
        case Task::Schedule: {
            const std::vector<pioneer::DeliveryInterval> deliveries =
                pioneer::parseSchedulingInput(in);
            return pioneer::renderScheduling(pioneer::solveScheduling(deliveries), machine);
        }
        case Task::Knapsack: {
            const pioneer::KnapsackInput input = pioneer::parseKnapsackInput(in);
            return pioneer::renderKnapsack(pioneer::solveKnapsack(input), machine);
        }
        case Task::Cleanup: {
            const pioneer::CleanupInput input = pioneer::parseCleanupInput(in);
            return pioneer::renderCleanup(pioneer::solveCleanup(input), machine);
        }
    }
    throw std::logic_error("unknown task");
}

int runSubcommand(Task task, const std::vector<std::string>& args) {
    const std::string name(taskName(task));
    const std::string usage = taskUsage(task);
    bool machine = false;
    std::optional<std::string> inputPath;

    for (std::size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        if (arg == "-h" || arg == "--help") {
            std::cout << helpText(task);
            return kExitOk;
        }
        if (arg == "--machine") {
            machine = true;
        } else if (arg == "--input") {
            if (i + 1 == args.size()) {
                return reportUsage("pioneer " + name + ": --input needs a PATH", usage);
            }
            // A repeated --input keeps the last path.
            inputPath = args[i + 1];
            ++i;
        } else {
            return reportUsage("pioneer " + name + ": unknown argument '" + arg + "'", usage);
        }
    }

    std::string text;
    if (!readInputText(inputPath, text)) {
        return kExitOperational;
    }
    const std::string encodingProblem = prepareInputText(text);
    if (!encodingProblem.empty()) {
        std::cerr << "pioneer " << name << ": " << encodingProblem << '\n';
        return kExitUsage;
    }

    try {
        std::cout << runTask(task, machine, text);
        return kExitOk;
    } catch (const pioneer::InputError& error) {
        std::cerr << error.what() << '\n';
        return kExitUsage;
    } catch (const std::exception& error) {
        std::cerr << "pioneer: " << error.what() << '\n';
        return kExitOperational;
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    if (args.empty()) {
        std::cout << kRootHelp;
        return kExitOk;
    }

    const std::string& first = args.front();
    if (first == "-h" || first == "--help") {
        std::cout << kRootHelp;
        return kExitOk;
    }

    const std::vector<std::string> rest(args.begin() + 1, args.end());
    if (const std::optional<Task> task = findTask(first)) {
        return runSubcommand(*task, rest);
    }
    return reportUsage("pioneer: unknown command '" + first + "'",
                       "usage: pioneer {schedule|knapsack|cleanup} [--input PATH] [--machine]\n"
                       "       pioneer --help\n");
}
