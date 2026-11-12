#pragma once

#include <iostream>
#include <string>
#include <string_view>

namespace pioneer::testing {

// Counts checks and prints each failure to stderr. Each test executable returns
// nonzero from main when any check failed.
class Checker {
public:
    void expect(bool condition, const std::string& description) {
        ++checks_;
        if (!condition) {
            ++failures_;
            std::cerr << "FAILED: " << description << '\n';
        }
    }

    // Passes when calling function throws Exception and its message contains fragment.
    template <typename Exception, typename Function>
    void expectThrows(Function function, std::string_view fragment, const std::string& description) {
        std::string message;
        bool thrown = false;
        try {
            function();
        } catch (const Exception& error) {
            thrown = true;
            message = error.what();
        }
        const bool matches = thrown && message.find(fragment) != std::string::npos;
        expect(matches, description + " (message: '" + message + "')");
    }

    // Prints a one-line summary and returns the number of failed checks.
    int report(std::string_view suiteName) const {
        std::cout << suiteName << ": " << (checks_ - failures_) << " of " << checks_
                  << " checks passed\n";
        return failures_;
    }

private:
    int checks_ = 0;
    int failures_ = 0;
};

}  // namespace pioneer::testing
