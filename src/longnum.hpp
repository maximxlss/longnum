#ifndef HEADER_LONGNUM
#define HEADER_LONGNUM

#include <vector>
#include <cstdint>
#include <iostream>
#include <string>
#include <format>

const int DEFAULT_PRECISION = 64;

class LongNum {
    int sign = 1;
    unsigned int binary_point = DEFAULT_PRECISION;
    std::vector<uint32_t> limbs;

    LongNum(int _sign, unsigned int _binary_point, std::vector<uint32_t> _limbs);

    inline void verify_invariants() const;
    inline void fix_invariants();

public:
    LongNum() = default;
    ~LongNum() = default;
    LongNum(const LongNum&) = default;
    LongNum(LongNum&&) = default;
    LongNum& operator=(const LongNum& other) = default;
    LongNum& operator=(LongNum&& other) = default;

    LongNum(long double value);

    std::strong_ordering operator<=>(const LongNum& rhs) const;
    bool operator==(const LongNum& rhs) const;

    LongNum& operator+=(const LongNum& rhs);
    friend LongNum operator+(LongNum lhs, const LongNum& rhs);

    LongNum operator-() const;
    LongNum& operator-=(const LongNum& rhs);
    friend LongNum operator-(LongNum lhs, const LongNum& rhs);

    // this coincides with multiplication/division by a power of two!
    // internally just a shift
    LongNum& operator<<=(int rhs);
    friend LongNum operator<<(LongNum lhs, int rhs);
    LongNum& operator>>=(int rhs);
    friend LongNum operator>>(LongNum lhs, int rhs);
    
    friend LongNum operator*(LongNum lhs, const LongNum& rhs);
    LongNum& operator*=(const LongNum& rhs);

    LongNum& operator/=(const LongNum& rhs);
    friend LongNum operator/(LongNum lhs, const LongNum& rhs);

    // bits left of the binary point are adressed by negative indicies
    bool get_bit(int pos) const;
    void set_bit(int pos);
    void unset_bit(int pos);

    // for numbers >= 1 it's the bit length of the whole part
    // for others it counts the number of zeros, but negative
    int bit_length() const;

    LongNum pow(int e) const;
    LongNum truncate() const;
    LongNum frac() const;
    LongNum round() const;

    int to_int() const;
    
    std::string to_binary_string() const;
    static LongNum from_binary_string(const std::string& number);

    std::string to_string(unsigned int base = 10) const;
    static LongNum from_string(const std::string& number, unsigned int base = 10);

    unsigned int precision() const;
    void set_precision(unsigned int precision);
    LongNum with_precision(unsigned int precision) const;
};

template <>
struct std::formatter<LongNum> : std::formatter<std::string> {
    auto format(const LongNum& number, std::format_context& ctx) const {
        return std::formatter<std::string>::format(number.to_string(), ctx);
    }
};

std::ostream& operator<<(std::ostream& stream, const LongNum& number);

LongNum operator""_longnum(long double value);
LongNum operator""_longnum(unsigned long long value);

// this is binary
LongNum operator""_longnum(const char* number, std::size_t len);

LongNum operator""_longdecimal(const char* number, std::size_t len);

#endif
