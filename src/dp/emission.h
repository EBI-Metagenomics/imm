#ifndef DP_EMISSION_H
#define DP_EMISSION_H

#include "imm/float.h"

struct imm_state;
struct code;

struct emission
{
    imm_float *score; /**< Sequence emission score of a state. */
    unsigned *offset; /**< Maps state to score array offset. */
};

void emission_del(struct emission const *emission);

void emission_init(struct emission *emission);

void emission_reset(struct emission *emission, struct code const *code,
                    struct imm_state **states, unsigned nstates);

static inline imm_float emission_score(struct emission const *emission,
                                       unsigned state, unsigned seq_code)
{
    return emission->score[emission->offset[state] + seq_code];
}

static inline unsigned emission_score_size(struct emission const *emission,
                                           unsigned nstates)
{
    return emission->offset[nstates];
}

static inline unsigned emission_offset_size(unsigned nstates)
{
    return nstates + 1;
}

#endif
