# LongNum arbitrary precision library

First-year educational project done at HSE SE: C++ arbitrary precision library. Supports handling fixed-point arithmetic of arbitrary precision, kinda fast and questionably safe 🤔

### Description
A class `LongNum` that implements fixed-point arithmetic (`+`, `-`, `*`, `/`) with selectable precision (the sizes of the numbers are unbounded though). Internally uses 32-bit limbs for not-terribly-slow computations. See [header file](./src/longnum.hpp) for details about the class exterior.

### Warning
Because I love using new stuff for no reason, this _requires_ **GCC 14** or newer.

On ubuntu: `sudo apt install g++-14`

### Makefile
Main targets: `run`, `run.vargrind`, `run.time`, `test`, `test.valgrind`, `test.coverage`, `clean`. Some interesting commands:
- `make run` - build and run `longnum-bin.cpp` (by default calculates pi).
- `make test` - test the library.
- `make test.coverage` - test the tests coverage (currently 96% lines, 100% functions). Do clean before and after.

Prefix `make` with `DEBUG=0` to use release parameters (**dramatically** faster).

### Example
```C++
#include <iostream>
#include <print>
#include <assert.h>
#include "longnum.hpp"

int main() {
    // Create a number, do some operations in-place
    LongNum x = 1;
    x *= 2;
    x -= 1;
    x /= 10;

    // Create another from expression (builtin types are converted automatically)
    LongNum y = 2 * x + 123;

    assert(y / LongNum(10).pow(100) == 0);
    // Not enough precision!

    y.set_precision(500);
    assert(y / LongNum(10).pow(100) != 0);
    // The precision of the result is picked to be the maximum between operands
}
```

### Roadmap
- Make limbs lazily allocated or copied for a dramatic speedup.
- Implement Karatsuba multiplication and some division algorithm, maybe.
