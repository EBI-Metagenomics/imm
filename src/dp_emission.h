#ifndef DP_EMISSION_H
#define DP_EMISSION_H

#include "score.h"
#include <inttypes.h>
#include <stdio.h>

struct imm_io;
struct mstate;
struct seq_code;

struct dp_emission
{
    score_t*  score;  /**< Sequence emission score of a state. */
    uint16_t* offset; /**< Maps state to score array offset. */
};

struct dp_emission const* dp_emission_create(struct seq_code const*      seq_code,
                                             struct mstate const* const* mstates,
                                             uint_fast16_t               nstates);
void                      dp_emission_destroy(struct dp_emission const* emission);
void dp_emission_offsets_dump(struct dp_emission const* emission, uint_fast16_t nstates);
struct dp_emission const* dp_emission_read(FILE* stream);
static inline score_t     dp_emission_score(struct dp_emission const* emission, uint_fast16_t state,
                                            uint_fast16_t seq_code);
void dp_emission_scores_dump(struct dp_emission const* emission, uint_fast16_t nstates);
int  dp_emission_write(struct dp_emission const* emission, uint_fast16_t nstates, FILE* stream);

static inline score_t dp_emission_score(struct dp_emission const* emission, uint_fast16_t state,
                                        uint_fast16_t seq_code)
{
    return emission->score[emission->offset[state] + seq_code];
}

#endif
