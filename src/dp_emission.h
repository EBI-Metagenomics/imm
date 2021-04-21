#ifndef DP_EMISSION_H
#define DP_EMISSION_H

#include "imm/imm.h"
#include <inttypes.h>
#include <stdio.h>

struct imm_io;
struct seq_code;

struct dp_emission
{
    imm_float* score;  /**< Sequence emission score of a state. */
    uint32_t*  offset; /**< Maps state to score array offset. */
};

struct dp_emission const* dp_emission_create(struct seq_code const* seq_code, struct imm_state** states,
                                             unsigned nstates);
void                      dp_emission_destroy(struct dp_emission const* emission);
struct dp_emission const* dp_emission_read(FILE* stream);
static inline imm_float   dp_emission_score(struct dp_emission const* emission, unsigned state, unsigned seq_code);
int                       dp_emission_write(struct dp_emission const* emission, unsigned nstates, FILE* stream);

static inline imm_float dp_emission_score(struct dp_emission const* emission, unsigned state, unsigned seq_code)
{
    return emission->score[emission->offset[state] + seq_code];
}

#endif
