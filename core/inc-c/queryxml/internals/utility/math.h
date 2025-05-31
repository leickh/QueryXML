
#ifndef QUERYXML_MATH_H
#define QUERYXML_MATH_H

#include <stddef.h>
#include <stdint.h>

size_t qxml_min_sz(size_t left, size_t right);

/// @brief Returns the rightmost set bit in a uint_least64_t or zero
///        if the given value is all-zeroes.
///
/// @param value Value of which to get the most significant bit
int_least32_t qxml_most_significant_bit(
    uint_least64_t value
);

uint_least64_t qxml_floor_pow2_u64(
    uint_least64_t value
);

uint_least64_t qxml_ceil_pow2_u64(
    uint_least64_t value
);

#endif // QUERYXML_MATH_H
