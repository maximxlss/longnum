#ifndef HEADER_LONGNUM
#define HEADER_LONGNUM

#include <vector>
#include <cstdint>
#include <iostream>
#include <string>
#include <format>
#include "significand.hpp"

class LongNum {
    Significand limbs;

    // the exponent, EXP_ZERO is a marker for the whole number to be zero
    int _exp = EXP_ZERO;

    // is the number negative or not
    bool _is_negative = false;

public:
    static const int EXP_ZERO = std::numeric_limits<int>::min();

    LongNum() = default;
    LongNum(const LongNum&) = default;
    LongNum(LongNum&&) = default;
    ~LongNum() = default;

    LongNum& operator=(const LongNum&) = default;
    LongNum& operator=(LongNum&&) = default;

    LongNum(long double value);

    bool is_zero() const;
    bool is_negative() const;
    int exp() const;

    std::size_t precision() const;
    void set_precision(std::size_t precision);
    LongNum with_precision(std::size_t precision) &&;
    LongNum with_precision(std::size_t precision) const&;

    std::strong_ordering operator<=>(const LongNum& rhs) const;
    bool operator==(const LongNum& rhs) const;

    LongNum& operator+=(const LongNum& rhs);
    friend LongNum operator+(LongNum lhs, const LongNum& rhs);

    friend LongNum operator-(LongNum value);
    LongNum& operator-=(const LongNum& rhs);
    friend LongNum operator-(LongNum lhs, const LongNum& rhs);

    // you can interpret this as multiplication/division by a power of two
    LongNum& operator<<=(int rhs);
    friend LongNum operator<<(LongNum lhs, int rhs);
    LongNum& operator>>=(int rhs);
    friend LongNum operator>>(LongNum lhs, int rhs);
    
    friend LongNum operator*(const LongNum& lhs, const LongNum& rhs);
    LongNum& operator*=(const LongNum& rhs);

    LongNum& operator/=(const LongNum& rhs);
    friend LongNum operator/(LongNum lhs, const LongNum& rhs);

    LongNum pow(int e) const;
    LongNum truncate() const;
    LongNum frac() const;
    LongNum round() const;
    LongNum abs() const;

    int to_int() const;
    
    std::string to_binary_string() const;
    static LongNum from_binary_string(const std::string& number);

    std::string to_string(unsigned int base = 10) const;
    static LongNum from_string(const std::string& number, unsigned int base = 10);
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
