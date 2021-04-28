#ifndef PAIR_H
#define PAIR_H

#include "imm/state.h"
#include <assert.h>

struct pair
{
    struct
    {
        union
        {
            struct
            {
                imm_state_id_t src;
                imm_state_id_t dst;
            };
            uint32_t key;
        };
    } id;

    struct
    {
        struct
        {
            imm_state_idx_t src;
            imm_state_idx_t dst;
        };
    } idx;
};

static_assert(sizeof(struct pair) == 8, "struct pair must be packed");

#define PAIR_INIT(s, d)                                                        \
    (struct pair) { .id.src = (s), .id.dst = (d) }

static inline void pair_init(struct pair *pair, unsigned src_id,
                             unsigned dst_id)
{
    pair->id.src = (imm_state_id_t)src_id;
    pair->id.dst = (imm_state_id_t)dst_id;
    pair->idx.src = IMM_STATE_NULL_IDX;
    pair->idx.dst = IMM_STATE_NULL_IDX;
}

#endif
