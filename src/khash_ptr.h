#ifndef KHASH_PTR_H
#define KHASH_PTR_H

#include "lib/khash.h"
#include <assert.h>
#include <stdint.h>

/*
 * Source: Linux kernel.
 *
 * This hash multiplies the input by a large odd number and takes the
 * high bits.  Since multiplication propagates changes to the most
 * significant end only, it is essential that the high bits of the
 * product be used for the hash value.
 *
 * Chuck Lever verified the effectiveness of this technique:
 * http://www.citi.umich.edu/techreports/reports/citi-tr-00-1.pdf
 *
 * Although a random odd number will do, it turns out that the golden
 * ratio phi = (sqrt(5)-1)/2, or its negative, has particularly nice
 * properties.  (See Knuth vol 3, section 6.4, exercise 9.)
 *
 * These are the negative, (1 - phi) = phi**2 = (3 - sqrt(5))/2,
 * which is very slightly easier to multiply by and makes no
 * difference to the hash distribution.
 */
#define GOLDEN_RATIO_32 0x61C88647
#define GOLDEN_RATIO_64 0x61C8864680B583EBull

static inline khint_t   ptr_hash_func(void const* ptr);
static inline uintptr_t reverse_bits(uintptr_t v);

#define __ptr_hash_equal(a, b) ((a) == (b))

#define KHASH_MAP_INIT_PTR(name, khval_t) KHASH_INIT(name, void const*, khval_t, 1, ptr_hash_func, __ptr_hash_equal)

static inline khint_t ptr_hash_func(void const* ptr)
{
    uintptr_t val = (uintptr_t)ptr;
#if SIZEOF_INTPTR_T == 8
    return (khint_t)reverse_bits(val * GOLDEN_RATIO_64);
#elif SIZEOF_INTPTR_T == 4
    return (khint_t)reverse_bits(val * GOLDEN_RATIO_32);
#else
#error SIZEOF_INTPTR_T not 4 or 8
#endif
}

/* Source: http://graphics.stanford.edu/~seander/bithacks.html#BitReverseObvious */
static inline uintptr_t reverse_bits(uintptr_t v)
{
    unsigned long r = v;                        // r will be reversed bits of v; first get LSB of v
    int           s = sizeof(v) * CHAR_BIT - 1; // extra shift needed at end

    for (v >>= 1; v; v >>= 1) {
        r <<= 1;
        r |= v & 1;
        s--;
    }
    r <<= s; // shift when v's highest bits are zero
    return r;
}

#endif
