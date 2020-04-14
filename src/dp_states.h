#ifndef DP_STATES_H
#define DP_STATES_H

#include <inttypes.h>
#include <stdio.h>

struct mstate;
struct imm_state;
struct state_idx;

struct dp_states
{
    uint32_t nstates;
    uint8_t* min_seq;
    uint8_t* max_seq;
    double*  start_lprob;
    uint32_t end_state;
};

struct dp_states const* dp_states_create(struct mstate const* const* mstates, unsigned nstates,
                                         struct imm_state const* end_state,
                                         struct state_idx*       state_idx);

static inline unsigned dp_states_min_seq(struct dp_states const* states, unsigned state)
{
    return states->min_seq[state];
}

static inline unsigned dp_states_max_seq(struct dp_states const* states, unsigned state)
{
    return states->max_seq[state];
}

static inline unsigned dp_states_nstates(struct dp_states const* states)
{
    return states->nstates;
}

static inline double dp_states_start_lprob(struct dp_states const* states, unsigned state)
{
    return states->start_lprob[state];
}

static inline unsigned dp_states_end_state(struct dp_states const* states)
{
    return states->end_state;
}

void dp_states_destroy(struct dp_states const* states);

int dp_states_write(struct dp_states const* states, FILE* stream);

#endif
