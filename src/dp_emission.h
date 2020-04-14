#ifndef DP_EMISSION_H
#define DP_EMISSION_H

#include <inttypes.h>
#include <stdio.h>

struct seq_code;
struct mstate;

struct dp_emission
{
    double*   score;  /**< Sequence emission score of a state. */
    uint32_t* offset; /**< Maps state to score array offset. */
};

struct dp_emission const* dp_emission_create(struct seq_code const*      seq_code,
                                             struct mstate const* const* mstates,
                                             unsigned                    nstates);

static inline double dp_emission_score(struct dp_emission const* emission, unsigned state,
                                       unsigned seq_code)
{
    return emission->score[emission->offset[state] + seq_code];
}

void dp_emission_destroy(struct dp_emission const* emission);

static inline unsigned dp_emission_score_size(struct dp_emission const* emission,
                                              unsigned                  nstates)
{
    return emission->offset[nstates];
}
static inline unsigned dp_emission_offset_size(unsigned nstates) { return nstates + 1; }
int dp_emission_write(struct dp_emission const* emission, uint32_t nstates, FILE* stream);

#endif
