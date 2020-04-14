#include "dp_state_table.h"
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

struct dp_state_table const* dp_state_table_create(struct mstate const* const* mstates,
                                                   uint32_t                    nstates,
                                                   struct imm_state const*     end_state,
                                                   struct state_idx*           state_idx)
{
    struct dp_state_table* state_tbl = malloc(sizeof(struct dp_state_table));

    state_tbl->nstates = nstates;

    state_tbl->min_seq = malloc(sizeof(*state_tbl->min_seq) * nstates);
    state_tbl->max_seq = malloc(sizeof(*state_tbl->max_seq) * nstates);
    state_tbl->start_lprob = malloc(sizeof(*state_tbl->start_lprob) * nstates);

    for (uint32_t i = 0; i < nstates; ++i) {
        state_idx_add(state_idx, mstate_get_state(mstates[i]), i);
        state_tbl->min_seq[i] = cast_u8_u(imm_state_min_seq(mstate_get_state(mstates[i])));
        state_tbl->max_seq[i] = cast_u8_u(imm_state_max_seq(mstate_get_state(mstates[i])));
        state_tbl->start_lprob[i] = mstate_get_start(mstates[i]);
    }

    state_tbl->end_state = state_idx_find(state_idx, end_state);

    return state_tbl;
}

void dp_state_table_destroy(struct dp_state_table const* state_tbl)
{
    free_c(state_tbl->min_seq);
    free_c(state_tbl->max_seq);
    free_c(state_tbl->start_lprob);
    free_c(state_tbl);
}

int dp_state_table_write(struct dp_state_table const* state_tbl, FILE* stream)
{
    struct dp_states_chunk chunk = {.nstates = state_tbl->nstates,
                                    .min_seq = state_tbl->min_seq,
                                    .max_seq = state_tbl->max_seq,
                                    .start_lprob = state_tbl->start_lprob,
                                    .end_state = state_tbl->end_state};

    if (fwrite(&chunk.nstates, sizeof(chunk.nstates), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.min_seq, sizeof(*chunk.min_seq), state_tbl->nstates, stream) <
        state_tbl->nstates)
        return 1;

    if (fwrite(chunk.max_seq, sizeof(*chunk.max_seq), 1, stream) < state_tbl->nstates)
        return 1;

    if (fwrite(chunk.start_lprob, sizeof(*chunk.start_lprob), state_tbl->nstates, stream) <
        state_tbl->nstates)
        return 1;

    if (fwrite(&chunk.end_state, sizeof(chunk.end_state), 1, stream) < 1)
        return 1;

    return 0;
}
