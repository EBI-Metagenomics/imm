#ifndef STATE_PAIR_H
#define STATE_PAIR_H

#include <stdint.h>

union state_pair
{
    uint16_t ids[2];
    uint32_t key;
};

#define STATE_PAIR_INIT(src, dst)                                              \
    (union state_pair) { .ids[0] = (src), .ids[1] = (dst) }

static inline void state_pair_init(union state_pair *pair, uint16_t src,
                                   uint16_t dst)
{
    pair->ids[0] = src;
    pair->ids[1] = dst;
}

#endif
