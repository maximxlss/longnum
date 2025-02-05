#ifndef HEADER_UTILS
#define HEADER_UTILS

#include<format>

template<typename T>
void assert_eq(T x, T y) {
    if (x != y) {
        throw std::logic_error(std::format("Assertion failed: {} != {}", x, y));
    }
}

void assert(bool value) {
    if (!value) {
        throw std::logic_error(std::format("Assertion failed."));
    }
}

#endif
