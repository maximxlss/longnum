# LongNum arbitrary precision library

First-year educational project done at HSE SE: C++ arbitrary precision library. Supports handling ~~fixed-point~~ floating-point arithmetic of arbitrary precision, kinda fast and questionably safe 🤔

### ❕Floating-point branch
You are on a branch containing a rewritten library with floating-point arithmetic instead of fixed-point. I hoped for a big speedup, since this implementation does *way* less copying, but it seems I've made a mistake somewhere or something idk. This also is NOT what was asked from me (the implementation should be fixed-point) so this is now very much useless.

### Description
A class `LongNum` that implements ~~fixed-point~~ floating-point arithmetic (`+`, `-`, `*`, `/`) with selectable precision (the sizes of the numbers are unbounded though). Internally uses 32-bit limbs for not-terribly-slow computations. See [header file](./src/longnum.hpp) for details about the class exterior.

### Warning
Requires GCC 13 or newer.

### Makefile
Main targets: `run`, `run.valgrind`, `run.callgrind`, `run.time`, `test`, `test.valgrind`, `test.callgrind`, `pi`, `pi.time`, `clean`. Some interesting commands:
- `make pi DIGITS=100` - calculate 100 digits of pi.
- `make run` - build and run `longnum-bin.cpp` (by default calculates pi).
- `make test` - test the library.
- `COVERAGE=1 make test` - test the tests coverage (currently 96% lines, 100% functions).

Prefix `make` with `RELEASE=1` to use release parameters (**dramatically** faster).

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
}
```

### Roadmap
- Implement Karatsuba multiplication and some division algorithm, maybe.
