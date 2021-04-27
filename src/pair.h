#ifndef PAIR_H
#define PAIR_H

#include "imm/state.h"
#include <stdint.h>

struct __attribute__((__packed__)) pair
{
    struct __attribute__((__packed__))
    {
        union
        {
            struct __attribute__((__packed__))
            {
                stateid_t src;
                stateid_t dst;
            };
            uint32_t key;
        };
    } id;

    struct __attribute__((__packed__))
    {
        union
        {
            struct __attribute__((__packed__))
            {
                stateidx_t src;
                stateidx_t dst;
            };
            uint32_t key;
        };
    } idx;
};

#define PAIR_INIT(s, d)                                                        \
    (struct pair) { .id.src = (s), .id.dst = (d) }

static inline void pair_init(struct pair *pair, stateid_t src_id,
                             stateid_t dst_id)
{
    pair->id.src = src_id;
    pair->id.dst = dst_id;
    pair->idx.key = 0;
}

#endif
