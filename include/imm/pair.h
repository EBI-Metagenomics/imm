#ifndef IMM_PAIR_H
#define IMM_PAIR_H

#include "imm/state.h"
#include <assert.h>

struct imm_pair
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

static_assert(sizeof(struct imm_pair) == 8, "struct pair must be packed");

#define IMM_PAIR_INIT(s, d)                                                    \
    (struct imm_pair)                                                          \
    {                                                                          \
        .id.src = (imm_state_id_t)(s), .id.dst = (imm_state_id_t)(d)           \
    }

static inline void imm_pair_init(struct imm_pair *pair, unsigned src_id,
                                 unsigned dst_id)
{
    pair->id.src = (imm_state_id_t)src_id;
    pair->id.dst = (imm_state_id_t)dst_id;
    pair->idx.src = IMM_STATE_NULL_IDX;
    pair->idx.dst = IMM_STATE_NULL_IDX;
}

#endif
