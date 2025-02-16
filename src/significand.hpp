#ifndef HEADER_SIGNIFICAND
#define HEADER_SIGNIFICAND

#include <memory>
#include <vector>
#include <ranges>
#include <iterator>
#include <bit>

// class to store the significand
// manages the precision
// avoids copy when changing the precision
class Significand {
    // vector of limbs in big-endian
    std::shared_ptr<std::vector<uint32_t>> limbs = std::make_shared<std::vector<uint32_t>>();

    // taken precision
    // when limbs is shared, indicates what precision to read it with
    std::size_t taken_precision = 0;

    // number of bits of precision
    std::size_t _precision = 96;

    // get mask for last limb
    // should be <last limb> & get_mask() == <last limb>
    inline uint32_t get_mask() const {
        if (taken_precision % 32 == 0) {
            return (uint32_t)0xFFFFFFFF;
        } else {
            return ~((uint32_t)0xFFFFFFFF >> (taken_precision % 32));
        }
    }

    inline std::size_t taken_size() const {
        return (taken_precision + 31) / 32;
    }

public:
    Significand() = default;
    Significand(const Significand&) = default;
    Significand(Significand&&) = default;
    ~Significand() = default;

    Significand& operator=(const Significand&) = default;
    Significand& operator=(Significand&&) = default;

    Significand(std::vector<uint32_t>&& _limbs) :
        limbs(std::make_shared<std::vector<uint32_t>>(_limbs)),
        taken_precision(limbs->size() * 32),
        _precision(taken_precision)
    {}

    inline std::size_t precision() const {
        return _precision;
    }

    inline std::size_t size() const {
        return (precision() + 31) / 32;
    }

    inline uint32_t get(std::size_t idx) const {
        #ifndef NDEBUG
        if (idx >= size()) {
            throw std::invalid_argument("idx out of bounds");
        }
        #endif
        if (idx >= taken_size()) {
            return 0;
        } else if (idx == taken_size() - 1) {
            return limbs->at(idx) & get_mask();
        } else {
            return limbs->at(idx);
        }
    }

    inline uint32_t get_with_offset(int offset, std::size_t idx) const {
        int d = offset / 32 + idx;
        int r = offset % 32;
        if (offset < 0 && r != 0) {
            d -= 1;
            r += 32;
        }
        uint32_t result = 0;
        if (d >= 0 && d < (int)taken_size()) {
            result |= get(d) << r;
        }
        if (d + 1 >= 0 && d + 1 < (int)taken_size() && r != 0) {
            result |= get(d + 1) >> (32 - r);
        }
        // leading one
        if (d == -1) {
            result |= 1 << r;
        }
        return result;
    }

    inline bool get_bit(std::size_t n) const {
        int d = n / 32;
        int r = n % 32;
        return (get(d) & (1ull << (31 - r))) != 0;
    }

    inline void make_mutable() {
        if (limbs.use_count() > 1) {
            limbs = std::make_shared<std::vector<uint32_t>>(limbs->begin(), limbs->begin() + taken_size());
        }
        if (precision() != taken_precision) {
            if (taken_size() > 0) {
                limbs->at(taken_size() - 1) &= get_mask();
            }
            limbs->resize(size(), 0);
            taken_precision = precision();
        }
    }

    inline uint32_t& at(std::size_t idx) {
        #ifndef NDEBUG
        if (idx >= size()) {
            throw std::invalid_argument("idx out of bounds");
        }
        #endif
        make_mutable();
        return limbs->at(idx);
    }

    inline void set_bit(std::size_t n) {
        int d = n / 32;
        int r = n % 32;
        at(d) |= 1ull << (31 - r);
    }

    inline std::size_t leading_zeros() const {
        int i = 0;
        while (i < (int)taken_size() && get(i) == 0) {
            i++;
        }
        if (i == (int)taken_size()) {
            return std::numeric_limits<std::size_t>::max();
        }
        return i * 32 + std::countl_zero(get(i));
    }

    inline void remove_front_bits(std::size_t n) {
        std::size_t d = n / 32;
        std::size_t r = n % 32;
        if (r != 0) {
            uint32_t carry = 0;
            for (int i = size() - 1; i >= 0; i--) {
                uint32_t new_carry = get(i) >> (32 - r);
                at(i) = carry | (get(i) << r);
                carry = new_carry;
            }
        }
        if (d != 0) {
            for (int i = d; i < (int)size(); i++) {
                at(i - d) = get(i);
            }
            for (int i = size() - d; i < (int)size(); i++) {
                at(i) = 0;
            }
        }
    }

    inline void set_precision(std::size_t precision_) {
        _precision = precision_;
        if (limbs.use_count() == 1) {
            limbs->resize(taken_size(), 0);
            if (taken_size() > 0) {
                limbs->at(taken_size() - 1) &= get_mask();
            }
            limbs->resize(size(), 0);
            taken_precision = precision();
        } else {
            taken_precision = std::min(taken_precision, precision());
        }
    }

    inline Significand with_precision(std::size_t precision_) && {
        set_precision(precision_);
        return *this;
    }

    inline Significand with_precision(std::size_t precision_) const& {
        Significand result = *this;
        result.set_precision(precision_);
        return result;
    }

    inline bool is_zero() const {
        for (int i = 0; i < (int)taken_size(); i++) {
            if (get(i) != 0) {
                return false;
            }
        }
        return true;
    }

    inline void insert_front_zeros(std::size_t n) {
        std::size_t d = n / 32;
        std::size_t r = n % 32;
        if (r != 0) {
            uint32_t carry = 0;
            for (int i = 0; i < (int)size(); i++) {
                uint32_t new_carry = get(i) << (32 - r);
                at(i) = carry | (get(i) >> r);
                carry = new_carry;
            }
        }
        if (d != 0) {
            for (int i = size() - 1 - d; i >= 0; i--) {
                at(i + d) = get(i);
            }
            for (int i = 0; i < (int)std::min(d, size()); i++) {
                at(i) = 0;
            }
        }
    }
};

#endif
