#ifndef DP_EMISSION_H
#define DP_EMISSION_H

#include "imm/float.h"

struct imm_state;
struct seq_code;

struct emission
{
    imm_float *score; /**< Sequence emission score of a state. */
    unsigned *offset; /**< Maps state to score array offset. */
};

void emission_del(struct emission const *emission);

struct emission const *emission_new(struct seq_code const *seq_code,
                                    struct imm_state **states,
                                    unsigned nstates);

static inline imm_float emission_score(struct emission const *emission,
                                       unsigned state, unsigned seq_code)
{
    return emission->score[emission->offset[state] + seq_code];
}

#endif
