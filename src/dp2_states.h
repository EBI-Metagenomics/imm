#ifndef IMM_DP2_STATES_H
#define IMM_DP2_STATES_H

struct mstate;
struct imm_state;
struct state_idx;

struct dp2_states
{
    unsigned* min_seq;
    unsigned* max_seq;
    double*   start_lprob;
    unsigned  nstates;
    unsigned  end_state;
};

struct dp2_states const* dp2_states_create(struct mstate const* const* mstates,
                                           unsigned nstates, struct imm_state const* end_state,
                                           struct state_idx* state_idx);

static inline unsigned dp2_states_min_seq(struct dp2_states const* states, unsigned state)
{
    return states->min_seq[state];
}

static inline unsigned dp2_states_max_seq(struct dp2_states const* states, unsigned state)
{
    return states->max_seq[state];
}

static inline unsigned dp2_states_nstates(struct dp2_states const* states)
{
    return states->nstates;
}

static inline double dp2_states_start_lprob(struct dp2_states const* states, unsigned state)
{
    return states->start_lprob[state];
}

static inline unsigned dp2_states_end_state(struct dp2_states const* states)
{
    return states->end_state;
}

void dp2_states_destroy(struct dp2_states const* states);

#endif
