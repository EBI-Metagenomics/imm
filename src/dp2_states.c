#include "dp2_states.h"
#include "imm/state.h"
#include "mstate.h"
#include "state_idx.h"
#include "free.h"
#include <stdlib.h>

struct dp2_states const* dp2_states_create(struct mstate const* const* mstates,
                                           unsigned nstates, struct imm_state const* end_state,
                                           struct state_idx* state_idx)
{
    struct dp2_states* states = malloc(sizeof(struct dp2_states));

    states->nstates = nstates;

    states->min_seq = malloc(sizeof(unsigned) * nstates);
    states->max_seq = malloc(sizeof(unsigned) * nstates);
    states->start_lprob = malloc(sizeof(double) * nstates);

    for (unsigned i = 0; i < nstates; ++i) {
        state_idx_add(state_idx, mstate_get_state(mstates[i]), i);
        states->min_seq[i] = imm_state_min_seq(mstate_get_state(mstates[i]));
        states->max_seq[i] = imm_state_max_seq(mstate_get_state(mstates[i]));
        states->start_lprob[i] = mstate_get_start(mstates[i]);
    }

    states->end_state = state_idx_find(state_idx, end_state);

    return states;
}

void dp2_states_destroy(struct dp2_states const* states)
{
    imm_free(states->min_seq);
    imm_free(states->max_seq);
    imm_free(states->start_lprob);
    imm_free(states);
}
