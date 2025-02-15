#include <iostream>
#include <assert.h>
#include <format>
#include"../src/longnum.hpp"


LongNum calculate_pi(unsigned int precision, unsigned int terms) {
    LongNum result;
    result.set_precision(precision);

    LongNum term = (2_longnum).with_precision(precision);
    result += term;

    // source: https://math.stackexchange.com/questions/14113/series-that-converge-to-pi-quickly
    for (int n = 1; n < terms; n++) {
        term >>= 1;
        term *= n * 2;
        term /= n * 2 + 1;
        result += term;
        
    }

    return result;
}

int main() {
    LongNum x = calculate_pi(350, 350);

    std::cout << x << std::endl;
}