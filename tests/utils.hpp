#ifndef HEADER_UTILS
#define HEADER_UTILS

#include<format>
#include<source_location>
#include<atomic>
#include<sstream>

static std::atomic<unsigned int> ALL_ASSERTIONS_NUMBER = 0;
static std::atomic<unsigned int> FAIL_ASSERTIONS_NUMBER = 0;

void assert(bool value, std::string message = "", std::source_location loc = std::source_location::current()) {
    ALL_ASSERTIONS_NUMBER++;
    if (!value) {
        FAIL_ASSERTIONS_NUMBER++;
    }
    std::string status = value ? "\033[30;42m<OK>\033[0m" : "\033[30;41m<FAIL>\033[0m";
    std::cout << "Test " << ALL_ASSERTIONS_NUMBER << " `" << loc.function_name() << "`: " << status;
    std::cout << " [" << loc.file_name() << ":" << loc.line() << "]";
    if (!value && !message.empty()) {
        std::cout << "\n    (" << message << ")";
    }
    std::cout << std::endl;
}

template<typename X, typename Y> requires requires(X x, Y y) { x == y; }
void assert_eq(X x, Y y, std::source_location loc = std::source_location::current()) {
    std::stringstream message;
    if constexpr (requires { message << x; message << y; }) {
        message << x;
        message << " != ";
        message << y;
    }
    assert(x == y, message.str(), loc);
}

void summary() {
    if (FAIL_ASSERTIONS_NUMBER == 0) {
        std::cout << "\033[30;42mSUCCESS\033[0m: No failed assertions.\n";
    } else {
        std::cout << "\033[30;41mFAIL\033[0m: " << FAIL_ASSERTIONS_NUMBER << " assertions failed.\n";
    }
    std::cout << (ALL_ASSERTIONS_NUMBER - FAIL_ASSERTIONS_NUMBER) << "/" << ALL_ASSERTIONS_NUMBER;
    std::cout << " tests OK." << std::endl;
}

#endif
