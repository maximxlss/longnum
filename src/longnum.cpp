#include "longnum.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>
#include <ranges>
#include<sstream>


void add_limbs(uint32_t& lhs, uint32_t rhs, int& carry) {
    uint64_t result = (uint64_t)lhs + rhs + carry;
    lhs = result;
    carry = result >> 32;
}

void sub_limbs(uint32_t& lhs, uint32_t rhs, int& carry) {
    int new_carry = lhs < rhs || (lhs <= rhs && carry);
    lhs -= rhs;
    lhs -= carry;
    carry = new_carry;
}

LongNum::LongNum(long double value) {
    if (value == 0) {
        return;
    }

    int exponent;
    value = std::frexp(value, &exponent);
    _exp = exponent - 1;
    
    if (value < 0) {
        _is_negative = true;
        value = -value;
    }

    value = std::ldexp(value, 1) - 1;
    
    std::vector<uint32_t> _limbs;
    while (value) {
        value = std::ldexp(value, 32);
        long double float_limb;
        value = std::modf(value, &float_limb);
        _limbs.emplace_back(float_limb);
    }
    limbs = Significand(std::move(_limbs));

    if constexpr (std::numeric_limits<long double>::radix == 2) {
        set_precision(std::numeric_limits<long double>::digits);
    } else {
        // (will never happen) fallback to approximate precision
        set_precision(std::numeric_limits<long double>::digits10 * 4);
    }
}

bool LongNum::is_zero() const {
    return _exp == EXP_ZERO;
}

bool LongNum::is_negative() const {
    return _is_negative;
}

int LongNum::exp() const {
    return _exp;
}

std::size_t LongNum::precision() const {
    return limbs.precision();
}

void LongNum::set_precision(std::size_t precision) {
    limbs.set_precision(precision);
}

LongNum LongNum::with_precision(std::size_t precision) && {
    set_precision(precision);
    return *this;
}

LongNum LongNum::with_precision(std::size_t precision) const& {
    LongNum result = *this;
    result.set_precision(precision);
    return result;
}

std::strong_ordering LongNum::operator<=>(const LongNum& rhs) const {
    if (is_zero() && !rhs.is_zero()) {
        return rhs.is_negative() ? std::strong_ordering::greater : std::strong_ordering::less;
    } else if (!is_zero() && rhs.is_zero()) {
        return is_negative() ? std::strong_ordering::less : std::strong_ordering::greater;
    } else if (is_zero() && rhs.is_zero()) {
        return std::strong_ordering::equal;
    } else if (is_negative() != rhs.is_negative()) {
        return is_negative() ? std::strong_ordering::less : std::strong_ordering::greater;;
    } else if (exp() != rhs.exp()) {
        return (exp() > rhs.exp()) ^ is_negative() ? std::strong_ordering::greater : std::strong_ordering::less;
    }
    std::size_t cmp_precision = std::max(precision(), rhs.precision());
    Significand cmp_left = limbs.with_precision(cmp_precision);
    Significand cmp_right = rhs.limbs.with_precision(cmp_precision);
    std::size_t max_i = cmp_left.size();
    for (int i = 0; i < (int)max_i; i++) {
        if (cmp_left.get(i) != cmp_right.get(i)) {
            return (cmp_left.get(i) > cmp_right.get(i)) ^ is_negative() ? std::strong_ordering::greater : std::strong_ordering::less;
        }
    }
    return std::strong_ordering::equal;
}

bool LongNum::operator==(const LongNum& rhs) const {
    if (is_zero() && rhs.is_zero()) {
        return true;
    } else if (is_zero() || rhs.is_zero()) {
        return false;
    } else if (is_negative() != rhs.is_negative()) {
        return false;
    } else if (exp() != rhs.exp()) {
        return false;
    }
    std::size_t cmp_precision = std::max(precision(), rhs.precision());
    Significand cmp_left = limbs.with_precision(cmp_precision);
    Significand cmp_right = rhs.limbs.with_precision(cmp_precision);
    std::size_t max_i = cmp_left.size();
    for (int i = 0; i < (int)max_i; i++) {
        if (cmp_left.get(i) != cmp_right.get(i)) {
            return false;
        }
    }
    return true;
}

LongNum& LongNum::operator+=(const LongNum& rhs) {
    if (precision() < rhs.precision()) {
        set_precision(rhs.precision());
    }
    if (rhs.is_zero()) {
        return *this;
    }
    if (is_zero()) {
        *this = rhs.with_precision(precision());
        return *this;
    }
    // ensure same sign or fall back to subtraction
    if (is_negative() != rhs.is_negative()) {
        *this -= -rhs;
        return *this;
    }
    // add the one from the rhs to make exp() >= rhs.exp()
    if (rhs.exp() > exp()) {
        limbs.insert_front_zeros(rhs.exp() - exp());
        _exp = rhs.exp();
    }
    int carry = 0;
    for (int i = limbs.size() - 1; i >= 0; i--) {
        add_limbs(limbs.at(i), rhs.limbs.get_with_offset(rhs.exp() - exp(), i), carry);
    }
    // leading one was missed
    if (rhs.exp() == exp()) {
        carry += 1;
    }
    if (carry >= 1) {
        limbs.insert_front_zeros(1);
        _exp += 1;
    }
    if (carry == 2) {
        limbs.set_bit(0);
    }
    return *this;
}

LongNum operator+(LongNum lhs, const LongNum& rhs) {
    lhs += rhs;
    return lhs;
}

LongNum operator-(LongNum value) {
    if (!value.is_zero()) {
        value._is_negative = !value._is_negative;
    }
    return value;
}

LongNum& LongNum::operator-=(const LongNum& rhs) {
    if (precision() < rhs.precision()) {
        set_precision(rhs.precision());
    }
    if (rhs.is_zero()) {
        return *this;
    }
    if (is_zero()) {
        *this = -rhs.with_precision(precision());
        return *this;
    }
    if (*this == rhs) {
        *this = (0_longnum).with_precision(precision());
        return *this;
    }
    // ensure same signs and |lhs| > |rhs| or fall back to addition
    if (is_negative() != rhs.is_negative()) {
        *this += -rhs;
        return *this;
    }
    if (abs() < rhs.abs()) {
        *this = -(rhs.with_precision(precision()) - *this);
        return *this;
    }
    // assume exp() >= rhs.exp() because |lhs| > |rhs|
    int carry = 0;
    for (int i = limbs.size() - 1; i >= 0; i--) {
        sub_limbs(limbs.at(i), rhs.limbs.get_with_offset(rhs.exp() - exp(), i), carry);
    }
    // leading one was missed
    if (rhs.exp() == exp()) {
        carry += 1;
    }
    assert(carry <= 1);
    if (carry == 1) {
        std::size_t leading_zeros = limbs.leading_zeros();
        limbs.remove_front_bits(leading_zeros + 1);
        _exp -= leading_zeros + 1;
    }
    return *this;
}

LongNum operator-(LongNum lhs, const LongNum& rhs) {
    lhs -= rhs;
    return lhs;
}

LongNum& LongNum::operator<<=(int n) {
    _exp += n;
    return *this;
}

LongNum operator<<(LongNum lhs, int n) {
    lhs <<= n;
    return lhs;
}

LongNum& LongNum::operator>>=(int n) {
    _exp -= n;
    return *this;
}

LongNum operator>>(LongNum lhs, int n) {
    lhs >>= n;
    return lhs;
}

LongNum operator*(const LongNum& lhs, const LongNum& rhs) {
    LongNum result = rhs;
    if (lhs.is_zero() || rhs.is_zero()) {
        return (0_longnum).with_precision(std::max(lhs.precision(), rhs.precision()));
    }
    result._exp += lhs.exp();
    result.set_precision(lhs.precision() + rhs.precision() + 32);
    int big_carry = 0;
    for (int i = lhs.limbs.size() - 1; i >= 0; i--) {
        for (int j = rhs.limbs.size() - 1; j >= -1; j--) {
            uint64_t mult = j >= 0 ? rhs.limbs.get(j) : 1;
            uint64_t limb_result = (uint64_t)result.limbs.get(i + j + 1) + (uint64_t)lhs.limbs.get(i) * mult;
            result.limbs.at(i + j + 1) = limb_result;
            if (i + j < 0) {
                big_carry += limb_result >> 32;
                continue;
            }
            int carry = 0;
            add_limbs(result.limbs.at(i + j), limb_result >> 32, carry);
            for (int k = i + j - 1; carry && k >= 0; k--) {
                add_limbs(result.limbs.at(k), 0, carry);
            }
            big_carry += carry;
        }
    }
    assert(big_carry <= 2);
    if (big_carry >= 1) {
        result.limbs.insert_front_zeros(1);
        result._exp += 1;
    }
    if (big_carry == 2) {
        result.limbs.set_bit(0);
    }
    result.set_precision(std::max(lhs.precision(), rhs.precision()));
    result._is_negative = lhs.is_negative() ^ rhs.is_negative();
    return result;
}

LongNum& LongNum::operator*=(const LongNum& rhs) {
    *this = *this * rhs;
    return *this;
}

LongNum& LongNum::operator/=(const LongNum& rhs) {
    if (rhs.is_zero()) {
        throw std::invalid_argument("Division by zero.");
    }
    if (precision() < rhs.precision()) {
        set_precision(rhs.precision());
    }
    if (is_zero()) {
        return *this;
    }
    LongNum result;
    result.set_precision(std::max(precision(), rhs.precision()));
    result._is_negative = is_negative() ^ rhs.is_negative();
    result._exp = exp() - rhs.exp();
    if (is_negative() != rhs.is_negative()) {
        *this = -*this;
    }
    _exp = rhs.exp();
    bool leading_bit = abs() >= rhs.abs();
    if (leading_bit) {
        *this -= rhs;
    }
    for (int shift = 1; !is_zero() && shift < (int)result.precision(); shift++) {
        LongNum shifted = rhs >> shift;
        if (abs() >= shifted.abs()) {
            *this -= shifted;
            result.limbs.set_bit(shift - 1);
        }
    }
    if (!leading_bit) {
        result.limbs.remove_front_bits(1);
        result._exp -= 1;
    }
    *this = result;
    return *this;
}

LongNum operator/(LongNum lhs, const LongNum& rhs) {
    lhs /= rhs;
    return lhs;
}

LongNum LongNum::pow(int e) const {
    LongNum base = *this;
    LongNum result = 1;
    while (e != 0) {
        if (e & 1) {
            result *= base;
        }
        base *= base;
        e >>= 1;
    }
    return result;
}

LongNum LongNum::truncate() const {
    if (exp() < 0) {
        return 0;
    } else {
        return with_precision(exp()).with_precision(precision());
    }
}

LongNum LongNum::frac() const {
    return *this - this->truncate();
}

LongNum LongNum::round() const {
    LongNum result = this->truncate();
    if (*this > 0 && this->frac() >= 0.5) {
        result += 1;
    }
    if (*this < 0 && this->frac() <= -0.5) {
        result -= 1;
    }
    return result;
}

LongNum LongNum::abs() const {
    LongNum result = *this;
    if (result.is_negative()) {
        result = -result;
    }
    return result;
}

int LongNum::to_int() const {
    if (is_zero() || exp() < 0) {
        return 0;
    }
    if (exp() >= 31) {
        throw std::invalid_argument("Overflow");
    }
    int sign = is_negative() ? -1 : 1;
    uint32_t num = limbs.get_with_offset(-1, 0);
    num >>= 31 - exp();
    return (int)num * sign;
}

// simpler specialization for binary
std::string LongNum::to_binary_string() const {
    if (is_zero()) {
        return "0";
    }
    std::stringstream result_stream;
    if (is_negative()) {
        result_stream << '-';
    }
    if (exp() < 0) {
        result_stream << '0';
        result_stream << '.';
        for (int i = 0; i < -1-exp(); i++) {
            result_stream << '0';
        }
    }
    result_stream << '1';

    std::stringstream significand_stream;
    for (int i = 0; i < (int)limbs.size(); i++) {
        significand_stream << std::format("{:032b}", limbs.get(i));
    }
    std::string significand_str = significand_stream.str().substr(0, precision());
    if (exp() >= 0 && exp() < (int)significand_str.size()) {
        significand_str.insert(significand_str.begin() + exp(), '.');
    }

    result_stream << significand_str;
    if (exp() >= (int)significand_str.size()) {
        for (int i = significand_str.size(); i < exp() + 1; i++) {
            result_stream << '0';
        }
    }

    return result_stream.str();
}

// simpler specialization for binary 
LongNum LongNum::from_binary_string(const std::string& number) {
    const std::string ws = " \t\n\r\f\v";
    
    std::size_t i = number.find_first_not_of(ws);
    if (i == std::string::npos) {
        return LongNum();
    }
    int sign = number[i] == '-' ? -1 : 1;
    if (number[i] == '+' || number[i] == '-') {
        i++;
    }
    int digits_start = i;
    std::size_t digits_end = number.find_last_not_of(ws);
    if (digits_end == number.size() - 1) {
        digits_end = std::string::npos;
    } else {
        digits_end += 1;
    }
    if (number.find_first_not_of("01.", digits_start) != digits_end) {
        throw std::invalid_argument(std::format("Invalid binary string: \"{}\"", number));
    }
    if (digits_end == std::string::npos) {
        digits_end = number.size();
    }
    std::vector<uint32_t> limbs;
    std::size_t binary_point = number.find_first_of('.');
    std::string lnum = number;
    if (binary_point != std::string::npos) {
        assert(binary_point < digits_end);
        lnum.erase(lnum.begin() + binary_point);
        binary_point = digits_end - 1 - binary_point;
        digits_end--;
    } else {
        binary_point = 0;
    }
    if (lnum.find_first_of(".") != std::string::npos) {
        throw std::invalid_argument(std::format("Invalid binary string: \"{}\"", number));
    }
    for (int i = digits_end - 1; i >= digits_start; i -= 32) {
        uint32_t limb = 0;
        for (int j = std::max(digits_start, i - 31); j <= i; j++) {
            limb <<= 1;
            limb |= lnum[j] == '1';
        }
        limbs.push_back(limb);
    }
    LongNum result;
    result.limbs = Significand(std::vector<uint32_t>(limbs.rbegin(), limbs.rend()));
    result._exp = 0;
    if (result == 1) {
        return 0;
    }
    result._is_negative = sign == -1;
    result._exp = result.limbs.precision() - binary_point;
    std::size_t leading_zeros = result.limbs.leading_zeros();
    result.limbs.remove_front_bits(leading_zeros + 1);
    result._exp -= leading_zeros + 1;
    return result;
}

std::string LongNum::to_string(unsigned int base) const {
    if (base == 2) {
        return to_binary_string();
    }
    if (base < 2 || base > 16) {
        throw std::invalid_argument("Invalid base under 2 or over 16");
    }
    const std::string digits = "0123456789abcdef";
    std::string result;
    LongNum whole = this->truncate();
    if (whole != 0) {
        whole.set_precision(std::abs(exp()));
    }
    while (whole != 0) {
        LongNum d = (whole / base).truncate();
        int rem = std::abs((whole - d * base).to_int());
        whole = d;
        result.push_back(digits[rem]);
    }
    if (result.size() == 0) {
        result.push_back('0');
    }
    if (is_negative()) {
        result.push_back('-');
    }
    std::reverse(result.begin(), result.end());
    LongNum frac = this->frac();
    if (frac != 0) {
        result.push_back('.');
    }
    int num_frac_digits = (long double)(precision() - exp()) / std::log2l(base);
    for (int i = 0; i < num_frac_digits && frac > 0; i++) {
        frac *= base;
        int rem = frac.truncate().to_int();
        result.push_back(digits[std::abs(rem)]);
        frac -= rem;
    }
    return result;
}

LongNum LongNum::from_string(const std::string& number, unsigned int base) {
    if (base == 2) {
        return LongNum::from_binary_string(number);
    }
    if (base < 2 || base > 16) {
        throw std::invalid_argument("Invalid base under 2 or over 16");
    }
    std::string lnum = number; 
    std::transform(lnum.begin(), lnum.end(), lnum.begin(),
        [](unsigned char c){ return std::tolower(c); });
    const std::string ws = " \t\n\r\f\v";
    std::string digits = "0123456789abcdef";
    digits = digits.substr(0, base);
    
    std::size_t i = lnum.find_first_not_of(ws);
    if (i == std::string::npos) {
        return LongNum();
    }
    int sign = lnum[i] == '-' ? -1 : 1;
    if (lnum[i] == '+' || lnum[i] == '-') {
        i++;
    }
    int digits_start = i;
    std::size_t digits_end = lnum.find_last_not_of(ws);
    if (digits_end == lnum.size() - 1) {
        digits_end = std::string::npos;
    } else {
        digits_end += 1;
    }
    if (lnum.find_first_not_of(digits + ".", digits_start) != digits_end) {
        throw std::invalid_argument(std::format("Invalid number string: \"{}\"", number));
    }
    if (digits_end == std::string::npos) {
        digits_end = lnum.size();
    }
    std::size_t point = number.find_first_of('.');
    if (point != std::string::npos) {
        assert(point < digits_end);
        lnum.erase(lnum.begin() + point);
        point = digits_end - 1 - point;
        digits_end--;
    } else {
        point = 0;
    }
    if (lnum.find_first_of(".") != std::string::npos) {
        throw std::invalid_argument(std::format("Invalid binary string: \"{}\"", number));
    }
    LongNum result;
    result.set_precision(std::log2l(base) * (digits_end - digits_start + 1));
    for (int i = digits_start; i < (int)digits_end; i++) {
        std::size_t idx = digits.find_first_of(lnum[i]);
        assert(idx != std::string::npos);
        result *= base;
        result += idx;
    }
    result /= LongNum(base).pow(point);
    if (!result.is_zero()) {
        result._is_negative = sign == -1;
    }
    return result;
}

std::ostream& operator<<(std::ostream& stream, const LongNum& number) {
    return stream << number.to_string();
}

LongNum operator""_longnum(long double value) {
    return LongNum(value);
};

LongNum operator""_longnum(unsigned long long value) {
    return LongNum((long double) value);
};

LongNum operator""_longnum(const char* number, std::size_t len) {
    return LongNum::from_binary_string(std::string(number, len));
};

LongNum operator""_longdecimal(const char* number, std::size_t len) {
    return LongNum::from_string(std::string(number, len), 10);
};
