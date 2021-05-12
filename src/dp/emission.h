#ifndef DP_EMISSION_H
#define DP_EMISSION_H

#include "imm/float.h"
#include <stdio.h>

struct imm_state;
struct code;

struct emission
{
    imm_float *score; /**< Sequence emission score of a state. */
    unsigned *offset; /**< Maps state to score array offset. */
};

void emission_deinit(struct emission const *emission);

void emission_init(struct emission *emission, struct code const *code,
                   struct imm_state **states, unsigned nstates);

void emission_reset(struct emission *emission, struct code const *code,
                    struct imm_state **states, unsigned nstates);

void emission_write(struct emission const *e, unsigned nstates, FILE *stream);

int emission_read(struct emission *e, FILE *stream);

static inline imm_float emission_score(struct emission const *emission,
                                       unsigned state, unsigned seq_code)
{
    return emission->score[emission->offset[state] + seq_code];
}

#endif
