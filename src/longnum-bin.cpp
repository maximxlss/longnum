#include <iostream>
#include <print>
#include <assert.h>
#include"../src/longnum.hpp"


LongNum calculate_pi(unsigned int precision, unsigned int terms) {
    LongNum result;
    result.set_precision(precision);

    // source: https://math.stackexchange.com/questions/14113/series-that-converge-to-pi-quickly
    for (int n = 0; n < terms; n++) {
        LongNum term = (1_longnum).with_precision(precision);
        term >>= (n - 1);
        for (int i = 1; i <= n; i++) {
            term *= i * 2;
            term /= i * 2 + 1;
        }
        result += term;
    }

    return result;
}

int main() {
    LongNum x = calculate_pi(350, 350);

    std::print("{}\n", x.to_binary_string());
}