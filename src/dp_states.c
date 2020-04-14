#include "dp_states.h"
#include "cast.h"
#include "free.h"
#include "imm/state.h"
#include "mstate.h"
#include "state_idx.h"
#include <stdlib.h>

struct dp_states_chunk
{
    uint32_t nstates;
    uint8_t* min_seq;
    uint8_t* max_seq;
    double*  start_lprob;
    uint32_t end_state;
};

struct dp_states const* dp_states_create(struct mstate const* const* mstates, unsigned nstates,
                                         struct imm_state const* end_state,
                                         struct state_idx*       state_idx)
{
    struct dp_states* states = malloc(sizeof(struct dp_states));

    states->nstates = nstates;

    states->min_seq = malloc(sizeof(unsigned) * nstates);
    states->max_seq = malloc(sizeof(unsigned) * nstates);
    states->start_lprob = malloc(sizeof(double) * nstates);

    for (unsigned i = 0; i < nstates; ++i) {
        state_idx_add(state_idx, mstate_get_state(mstates[i]), i);
        states->min_seq[i] = cast_u8_u(imm_state_min_seq(mstate_get_state(mstates[i])));
        states->max_seq[i] = cast_u8_u(imm_state_max_seq(mstate_get_state(mstates[i])));
        states->start_lprob[i] = mstate_get_start(mstates[i]);
    }

    states->end_state = state_idx_find(state_idx, end_state);

    return states;
}

void dp_states_destroy(struct dp_states const* states)
{
    free_c(states->min_seq);
    free_c(states->max_seq);
    free_c(states->start_lprob);
    free_c(states);
}

int dp_states_write(struct dp_states const* states, FILE* stream)
{
    struct dp_states_chunk chunk = {.nstates = states->nstates,
                                    .min_seq = states->min_seq,
                                    .max_seq = states->max_seq,
                                    .start_lprob = states->start_lprob,
                                    .end_state = states->end_state};

    if (fwrite(&chunk.nstates, sizeof(chunk.nstates), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.min_seq, sizeof(*chunk.min_seq) * states->nstates, 1, stream) < 1)
        return 1;

    if (fwrite(chunk.max_seq, sizeof(*chunk.max_seq) * states->nstates, 1, stream) < 1)
        return 1;

    if (fwrite(chunk.start_lprob, sizeof(*chunk.start_lprob) * states->nstates, 1, stream) < 1)
        return 1;

    if (fwrite(&chunk.end_state, sizeof(chunk.end_state), 1, stream) < 1)
        return 1;

    return 0;
}
