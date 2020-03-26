#ifndef IMM_DP2_EMISSION_H
#define IMM_DP2_EMISSION_H

struct dp2_emission;
struct seq_code;
struct mstate;

/**
 * States are unsigned integers: 0, 1, ..., nstates-1.
 * Sequence code is an unsigned integer assigned to a particular sequence.
 * This assignment differs from state to state according to the size of
 * smallest sequence it can emit. Let min_seq(j) be that size for state j,
 * and let SC() be the sequence code function.
 * cost[offset[i] + SC(seq, min_seq(j))] will give the emission cost for
 * sequence seq.
 *
 */
struct dp2_emission
{
    double*   cost;   /**< Sequence emission cost of a state. */
    unsigned* offset; /**< Maps state to cost array offset. */
};

struct dp2_emission const* dp2_emission_create(struct seq_code const*      seq_code,
                                               struct mstate const* const* mstates,
                                               unsigned                    nstates);

static inline double dp2_emission_cost(struct dp2_emission const* emission, unsigned state,
                                       unsigned seq_code)
{
    return emission->cost[emission->offset[state] + seq_code];
}

void dp2_emission_destroy(struct dp2_emission const* emission);

#endif
