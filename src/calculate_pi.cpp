#include <iostream>
#include <format>
#include <cmath>
#include"../src/longnum.hpp"


LongNum calculate_pi(std::size_t precision) {
    LongNum result;
    result.set_precision(precision);

    LongNum term = (2_longnum).with_precision(precision);
    result += term;

    // source: https://math.stackexchange.com/questions/14113/series-that-converge-to-pi-quickly
    for (int n = 1; n <= (int)precision; n++) {
        term >>= 1;
        term *= n * 2;
        term /= n * 2 + 1;
        term.set_precision(precision - n + 2);
        result += term;
    }

    result.set_precision(precision);

    return result;
}

int main(int argc, char *argv[]) {
    int N_DIGITS = 100;
    if (argc > 1) {
        N_DIGITS = std::atoi(argv[1]);
    }

    LongNum x = calculate_pi((N_DIGITS + 2) * std::log2l(10));

    std::cout << x.to_string().substr(0, 2 + N_DIGITS) << std::endl;
}