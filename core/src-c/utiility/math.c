#include <queryxml/internals/utility/math.h>

size_t qxml_min_sz(size_t left, size_t right)
{
    if (left < right)
    {
        return left;
    }
    return right;
}

int_least32_t qxml_most_significant_bit(
    uint_least64_t value
) {
    int_least32_t rightmost = 63;
    while (rightmost >= 0)
    {
        uint_least8_t bit = (value >> rightmost) & 1;
        if (bit)
        {
            break;
        }
        --rightmost;
    }
    return rightmost;
}

uint_least64_t qxml_floor_pow2_u64(
    uint_least64_t value
) {
    return ((uint_least64_t) 1) << qxml_most_significant_bit(value);
}

uint_least64_t qxml_ceil_pow2_u64(
    uint_least64_t value
) {
    return qxml_floor_pow2_u64(value) * 2;
}
