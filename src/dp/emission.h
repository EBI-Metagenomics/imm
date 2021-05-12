#ifndef DP_EMISSION_H
#define DP_EMISSION_H

#include "imm/cartes.h"
#include "imm/float.h"

struct imm_state;
struct code;

struct emission
{
    struct
    {
        size_t score;
        size_t offset;
    } capacity;
    imm_float *score; /**< Sequence emission score of a state. */
    unsigned *offset; /**< Maps state to score array offset. */
    struct imm_cartes cartes;
};

void emission_deinit(struct emission const *emission);

void emission_init(struct emission *emission, struct code const *code,
                   struct imm_state **states, unsigned nstates);

void emission_reset(struct emission *emission, struct code const *code,
                    struct imm_state **states, unsigned nstates);

static inline imm_float emission_score(struct emission const *emission,
                                       unsigned state, unsigned seq_code)
{
    return emission->score[emission->offset[state] + seq_code];
}

#endif
