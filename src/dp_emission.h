#ifndef IMM_DP_EMISSION_H
#define IMM_DP_EMISSION_H

struct dp_emission;
struct seq_code;
struct mstate;

struct dp_emission
{
    double*   score;  /**< Sequence emission score of a state. */
    unsigned* offset; /**< Maps state to score array offset. */
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

#endif
