#include "longnum.hpp"
#include <cassert>
#include <cmath>

const int DEFAULT_PRECISION = 64;

inline void add_limbs(uint32_t& lhs, uint32_t rhs, int& carry) {
    uint64_t result = (uint64_t)lhs + rhs + carry;
    lhs = result;
    carry = result >> 32;
}

inline void sub_limbs(uint32_t& lhs, uint32_t rhs, int& carry) {
    int new_carry = lhs < rhs || (lhs <= rhs && carry);
    lhs -= rhs;
    lhs -= carry;
    carry = new_carry;
}

LongNum::LongNum(int _sign, unsigned int _binary_point, std::vector<uint32_t> _limbs) : sign(_sign), binary_point(_binary_point), limbs(std::move(_limbs))  {
    fix_invariants();
}

inline void LongNum::verify_invariants() const {
    #ifndef NDEBUG
    if (sign != 1 && sign != -1) {
        throw std::logic_error(std::format("Sign is not -1 and not 1; it's {}.", sign));
    }
    if (limbs.size() == 0 && sign != 1) {
        throw std::logic_error(std::format("Sign of zero is not 1; it's {}.", sign));
    }
    if (limbs.size() > 1 && limbs.back() == 0) {
        throw std::logic_error("Back limb is zero.");
    }
    #endif
}

inline void LongNum::fix_invariants() {
    while (limbs.size() > 0 && limbs.back() == 0) {
        limbs.pop_back();
    }
    if (limbs.size() == 0) {
        sign = 1;
    }
    verify_invariants();
}

LongNum::LongNum()
    : sign(1),
    binary_point(DEFAULT_PRECISION),
    limbs()
{
    verify_invariants();
}

LongNum::LongNum(long double value)
    : sign(1),
    binary_point(0),
    limbs()
{
    int exponent;
    value = std::frexp(value, &exponent);
    
    if (value < 0) {
        sign = -1;
        value = -value;
    }

    unsigned long long mantissa = scalb(value, std::numeric_limits<double>::digits);
    exponent -= std::numeric_limits<double>::digits;
    
    while (mantissa) {
        limbs.push_back(mantissa);
        mantissa >>= 32;
    }
    
    binary_point = -exponent;

    if (binary_point < DEFAULT_PRECISION) {
        set_precision(DEFAULT_PRECISION);
    }

    verify_invariants();
}


LongNum& LongNum::operator=(LongNum rhs) {
    std::swap(sign, rhs.sign);
    std::swap(binary_point, rhs.binary_point);
    std::swap(limbs, rhs.limbs);
    return *this;
}

std::strong_ordering LongNum::operator<=>(const LongNum& rhs) const {
    verify_invariants();
    if (sign > rhs.sign) {
        return std::strong_ordering::greater;
    } else if (sign < rhs.sign) {
        return std::strong_ordering::less;
    } else if (bit_length() > rhs.bit_length()) {
        return sign > 0 ? std::strong_ordering::greater : std::strong_ordering::less;
    } else if (bit_length() < rhs.bit_length()) {
        return sign > 0 ? std::strong_ordering::less : std::strong_ordering::greater;
    }
    if (binary_point > rhs.binary_point) {
        // fixme: slower but complicated to improve (see operator<<=)
        LongNum new_rhs = rhs;
        new_rhs.set_precision(binary_point);
        return *this <=> new_rhs;
    } else if (binary_point != rhs.binary_point) {
        LongNum new_lhs = *this;
        new_lhs.set_precision(rhs.binary_point);
        return new_lhs <=> rhs;
    }
    for (int i = limbs.size() - 1; i >= 0; i--) {
        if (limbs[i] > rhs.limbs[i]) {
            return sign > 0 ? std::strong_ordering::greater : std::strong_ordering::less;
        } else if (limbs[i] < rhs.limbs[i]) {
            return sign > 0 ? std::strong_ordering::less : std::strong_ordering::greater;
        }
    }
    return std::strong_ordering::equal;
}

bool LongNum::operator==(const LongNum& rhs) const {
    verify_invariants();
    rhs.verify_invariants();
    if (binary_point > rhs.binary_point) {
        // fixme: slower but complicated to improve (see operator<<=)
        LongNum new_rhs = rhs;
        new_rhs.set_precision(binary_point);
        return *this == new_rhs;
    } else if (binary_point != rhs.binary_point) {
        return rhs == *this;
    }
    return limbs == rhs.limbs && sign == rhs.sign;
}

LongNum& LongNum::operator+=(const LongNum& rhs) {
    verify_invariants();
    rhs.verify_invariants();
    if (rhs == 0) {
        return *this;
    }
    if (binary_point < rhs.binary_point) {
        set_precision(rhs.binary_point);
    }
    if (binary_point > rhs.binary_point) {
        // fixme: slower but complicated to improve (see operator<<=)
        LongNum new_rhs = rhs;
        new_rhs.set_precision(binary_point);
        *this += new_rhs;
        return *this;
    }
    // ensure sign lhs = sign rhs or fall back to subtraction
    if (sign != rhs.sign) {
        *this -= -rhs;
        return *this;
    }
    int carry = 0;
    for (int i = 0; i < (int)std::max(limbs.size(), rhs.limbs.size()); i++) {
        if (i >= (int)limbs.size()) {
            limbs.emplace_back(0);
        }
        add_limbs(limbs[i], i < (int)rhs.limbs.size() ? rhs.limbs[i] : 0, carry);
    }
    if (carry == 1) {
        limbs.emplace_back(1);
    }
    verify_invariants();
    rhs.verify_invariants();
    return *this;
}

LongNum operator+(LongNum lhs, const LongNum& rhs) {
    lhs.verify_invariants();
    rhs.verify_invariants();
    lhs += rhs;
    lhs.verify_invariants();
    rhs.verify_invariants();
    return lhs;
}

LongNum LongNum::operator-() const {
    // fixme: would be used inefficiently with a copy
    // even when not needed
    // maybe some kind of no-copy views
    verify_invariants();
    LongNum result(*this);
    if (result != 0) {
        result.sign = -result.sign;
    }
    result.verify_invariants();
    return result;
}

LongNum& LongNum::operator-=(const LongNum& rhs) {
    verify_invariants();
    rhs.verify_invariants();
    if (rhs == 0) {
        return *this;
    }
    if (*this == rhs) {
        *this = 0;
        return *this;
    }
    if (binary_point < rhs.binary_point) {
        set_precision(rhs.binary_point);
    }
    if (binary_point > rhs.binary_point) {
        // fixme: slower but complicated to improve (see operator<<=)
        LongNum new_rhs = rhs;
        new_rhs.set_precision(binary_point);
        *this -= new_rhs;
        return *this;
    }
    // ensure |lhs| > |rhs| or fall back to addition
    if (sign != rhs.sign) {
        *this += -rhs;
        return *this;
    }
    if ((*this < rhs) ^ (sign < 0)) {
        *this = -(rhs - *this);
        return *this;
    }
    int carry = 0;
    for (int i = 0; i < (int)limbs.size(); i++) {
        sub_limbs(limbs[i], i < (int)rhs.limbs.size() ? rhs.limbs[i] : 0, carry);
    }
    assert(carry == 0);
    fix_invariants();
    rhs.verify_invariants();
    return *this;
}

LongNum operator-(LongNum lhs, const LongNum& rhs) {
    lhs.verify_invariants();
    rhs.verify_invariants();
    lhs -= rhs;
    lhs.verify_invariants();
    rhs.verify_invariants();
    return lhs;
}

LongNum& LongNum::operator<<=(int n) {
    // fixme: used inefficiently with a copy in many places
    // maybe fix with "shifted" no-copy views?
    // or restricting binary_point to 32-multiples
    // though doesn't matter with a fixed precision
    verify_invariants();
    if (n == 0) {
        return *this;
    } else if (n < 0) {
        *this >>= -n;
        return *this;
    }
    int r = n % 32;
    if (r != 0) {
        uint32_t carry = 0;
        for (int i = 0; i < (int)limbs.size(); i++) {
            uint32_t new_carry = limbs[i] >> (32 - r);
            limbs[i] <<= r;
            limbs[i] |= carry;
            carry = new_carry;
        }
        if (carry) {
            limbs.emplace_back(carry);
        }
    }
    int d = n / 32;
    // fixme: slower but c++23 insert_range is not available for some reason
    for (int i = 0; i < d; i++) {
        limbs.emplace(limbs.begin(), 0);
    }
    fix_invariants();
    verify_invariants();
    return *this;
}

LongNum operator<<(LongNum lhs, int n) {
    lhs.verify_invariants();
    lhs <<= n;
    lhs.verify_invariants();
    return lhs;
}

LongNum& LongNum::operator>>=(int n) {
    verify_invariants();
    if (n == 0) {
        return *this;
    } else if (n < 0) {
        *this <<= -n;
        return *this;
    }
    int d = std::min(n / 32, (int)limbs.size());
    limbs.erase(limbs.begin(), limbs.begin() + d);
    int r = n % 32;
    if (r != 0) {
        uint32_t carry = 0;
        for (int i = limbs.size() - 1; i >= 0; i--) {
            uint32_t new_carry = limbs[i] << (32 - r);
            limbs[i] >>= r;
            limbs[i] |= carry;
            carry = new_carry;
        }
    }
    fix_invariants();
    return *this;
}

LongNum operator>>(LongNum lhs, int n) {
    lhs.verify_invariants();
    lhs >>= n;
    lhs.verify_invariants();
    return lhs;
}

LongNum operator*(LongNum lhs, const LongNum& rhs) {
    lhs.verify_invariants();
    rhs.verify_invariants();
    LongNum result;
    result.limbs.resize(lhs.limbs.size() + rhs.limbs.size(), 0);
    for (int i = 0; i < (int)rhs.limbs.size(); i++) {
        for (int j = 0; j < (int)lhs.limbs.size(); j++) {
            uint64_t limb_result = (uint64_t)result.limbs[i + j] + (uint64_t)lhs.limbs[j] * (uint64_t)rhs.limbs[i];
            result.limbs[i + j] = limb_result;
            int carry = 0;
            add_limbs(result.limbs[i + j + 1], limb_result >> 32, carry);
            for (int k = i + j + 2; k < (int)result.limbs.size(); k++) {
                add_limbs(result.limbs[k], 0, carry);
            }
        }
    }
    result.fix_invariants();
    if (result.limbs.size() != 0) {
        result.sign = lhs.sign * rhs.sign;
    }
    result.binary_point = lhs.binary_point + rhs.binary_point;
    result.set_precision(std::max(lhs.binary_point, rhs.binary_point));
    lhs.verify_invariants();
    rhs.verify_invariants();
    return result;
}

LongNum& LongNum::operator*=(const LongNum& rhs) {
    verify_invariants();
    rhs.verify_invariants();
    *this = *this * rhs;
    verify_invariants();
    rhs.verify_invariants();
    return *this;
}

LongNum& LongNum::operator/=(const LongNum& rhs) {
    verify_invariants();
    rhs.verify_invariants();
    if (rhs == 0) {
        throw std::invalid_argument("Division by zero.");
    }
    if (*this == 0) {
        return *this;
    }
    if (binary_point > rhs.binary_point) {
        LongNum new_rhs = rhs;
        new_rhs.set_precision(binary_point);
        *this /= new_rhs;
        return *this;
    }
    int result_sign = sign * rhs.sign;
    if (sign != rhs.sign) {
        *this = -*this;
    }
    
    LongNum result;
    result.set_precision(std::max(binary_point, rhs.binary_point));
    while (*this != 0) {
        int shift = bit_length() - rhs.bit_length();
        if (shift < -(int)result.binary_point) {
            break;
        }
        LongNum shifted = rhs << shift;
        if (((*this > shifted) ^ (sign < 0)) || (*this == shifted)) {
            *this -= shifted;
            result.set_bit(shift);
        } else {
            if (shift <= -(int)result.binary_point) {
                break;
            }
            shifted >>= 1;
            *this -= shifted;
            result.set_bit(shift - 1);
        }
    }
    result.sign = result_sign;
    result.fix_invariants();
    *this = result;
    verify_invariants();
    rhs.verify_invariants();
    return *this;
}

LongNum operator/(LongNum lhs, const LongNum& rhs) {
    lhs.verify_invariants();
    rhs.verify_invariants();
    lhs /= rhs;
    lhs.verify_invariants();
    rhs.verify_invariants();
    return lhs;
}

bool LongNum::get_bit(int pos) const {
    verify_invariants();
    pos += binary_point;
    if (pos < 0) {
        throw std::invalid_argument("Trying to get a bit out of bounds");
    }
    unsigned int d = pos / 32;
    unsigned int r = pos % 32;
    bool result = (limbs[d] >> r) & 1;
    verify_invariants();
    return result;
}

void LongNum::set_bit(int pos) {
    verify_invariants();
    pos += binary_point;
    if (pos < 0) {
        throw std::invalid_argument("Trying to get a bit out of bounds");
    }
    unsigned int d = pos / 32;
    unsigned int r = pos % 32;
    limbs.resize(std::max((unsigned int)limbs.size(), d + 1), 0);
    limbs[d] |= 1 << r;
    verify_invariants();
}

void LongNum::unset_bit(int pos) {
    verify_invariants();
    pos += binary_point;
    if (pos < 0) {
        throw std::invalid_argument("Trying to get a bit out of bounds");
    }
    unsigned int d = pos / 32;
    if (d < limbs.size()) {
        unsigned int r = pos % 32;
        limbs[d] &= ~((uint32_t)1 << r);
    }
    fix_invariants();
}

int LongNum::bit_length() const {
    verify_invariants();
    if (limbs.size() == 0) {
        return -(int)binary_point;
    }
    return ((int)limbs.size() - 1) * 32 + (32 - std::countl_zero(limbs.back())) - binary_point;
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
    LongNum result = *this;
    result.set_precision(0);
    result.set_precision(DEFAULT_PRECISION);
    return result;
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

int LongNum::to_int() const {
    int d = binary_point / 32;
    int r = binary_point % 32;
    uint32_t result = 0;
    if (d < (int)limbs.size()) {
        result |= limbs[d] >> r;
    }
    if (d + 1 < (int)limbs.size() && r != 0) {
        result |= limbs[d + 1] << (32 - r);
    }
    result &= 0x7FFFFFFF;
    return sign * (int) result;
}

// simpler specialization for binary
std::string LongNum::to_binary_string() const {
    verify_invariants();
    std::string result;
    for (int i = 0; i < (int)limbs.size(); i++) {
        uint64_t x = limbs[i];
        for (int j = 0; j < 32; j++) {
            if (x == 0 && i == (int)limbs.size() - 1) {
                break;
            }
            if (result.size() == binary_point) {
                result.push_back('.');
            }
            result.push_back(x & 1 ? '1' : '0');
            x >>= 1;
        }
    }
    while (result.size() < binary_point) {
        result.push_back('0');
    }
    if (result.size() == binary_point && binary_point != 0) {
        result.push_back('.');
        result.push_back('0');
    }
    if (result.empty()) {
        result.push_back('0');
    }
    if (sign < 0) {
        result.push_back('-');
    }
    return std::string(result.rbegin(), result.rend());
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
    LongNum result(sign, binary_point, std::move(limbs));
    if (binary_point < DEFAULT_PRECISION) {
        result.set_precision(DEFAULT_PRECISION);
    }
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
    while (whole != 0) {
        LongNum d = (whole / base).truncate();
        int rem = std::abs((whole - d * base).to_int());
        whole = d;
        result.push_back(digits[rem]);
    }
    if (result.size() == 0) {
        result.push_back('0');
    }
    if (sign < 0) {
        result.push_back('-');
    }
    std::reverse(result.begin(), result.end());
    LongNum frac = this->frac();
    if (frac != 0) {
        result.push_back('.');
    }
    while (frac != 0) {
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
    for (int i = digits_start; i < (int)digits_end; i++) {
        std::size_t idx = digits.find_first_of(lnum[i]);
        assert(idx != std::string::npos);
        LongNum d = idx;
        result *= base;
        result += d;
    }

    result.set_precision(4 * point);
    result /= LongNum(base).pow(point);
    if (result != 0) {
        result.sign = sign;
    }

    result.verify_invariants();
    return result;
}

unsigned int LongNum::precision() const {
    return binary_point;
}

void LongNum::set_precision(unsigned int precision) {
    *this <<= precision - binary_point;
    binary_point = precision;
}

LongNum LongNum::with_precision(unsigned int precision) const {
    LongNum result = *this;
    result.set_precision(precision);
    return result;
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
