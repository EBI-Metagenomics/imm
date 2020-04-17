#include "dp_state_table.h"
#include "cast.h"
#include "free.h"
#include "imm/state.h"
#include "io.h"
#include "mstate.h"
#include "state_idx.h"
#include <stdlib.h>

struct dp_state_table_chunk
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
    struct dp_state_table_chunk chunk = {.nstates = state_tbl->nstates,
                                         .min_seq = state_tbl->min_seq,
                                         .max_seq = state_tbl->max_seq,
                                         .start_lprob = state_tbl->start_lprob,
                                         .end_state = state_tbl->end_state};

    if (fwrite(&chunk.nstates, sizeof(chunk.nstates), 1, stream) < 1) {
        imm_error("could not write nstates");
        return 1;
    }

    if (fwrite(chunk.min_seq, sizeof(*chunk.min_seq), state_tbl->nstates, stream) <
        state_tbl->nstates) {
        imm_error("could not write min_seq");
        return 1;
    }

    if (fwrite(chunk.max_seq, sizeof(*chunk.max_seq), state_tbl->nstates, stream) <
        state_tbl->nstates) {
        imm_error("could not write max_seq");
        return 1;
    }

    if (fwrite(chunk.start_lprob, sizeof(*chunk.start_lprob), state_tbl->nstates, stream) <
        state_tbl->nstates) {
        imm_error("could not write start_lprob");
        return 1;
    }

    if (fwrite(&chunk.end_state, sizeof(chunk.end_state), 1, stream) < 1) {
        imm_error("could not write end_state");
        return 1;
    }

    return 0;
}

struct dp_state_table *dp_state_table_read(FILE* stream)
{
    struct dp_state_table_chunk chunk = {
        .nstates = 0, .min_seq = NULL, .max_seq = NULL, .start_lprob = NULL, .end_state = 0};

    struct dp_state_table* state_tbl = malloc(sizeof(*state_tbl));

    if (fread(&chunk.nstates, sizeof(chunk.nstates), 1, stream) < 1) {
        imm_error("could not read nstates");
        goto err;
    }

    chunk.min_seq = malloc(sizeof(*chunk.min_seq) * chunk.nstates);
    chunk.max_seq = malloc(sizeof(*chunk.max_seq) * chunk.nstates);
    chunk.start_lprob = malloc(sizeof(*chunk.start_lprob) * chunk.nstates);

    if (fread(chunk.min_seq, sizeof(*chunk.min_seq), chunk.nstates, stream) < chunk.nstates) {
        imm_error("could not read min_seq");
        goto err;
    }

    if (fread(chunk.max_seq, sizeof(*chunk.max_seq), chunk.nstates, stream) < chunk.nstates) {
        imm_error("could not read max_seq");
        goto err;
    }

    if (fread(chunk.start_lprob, sizeof(*chunk.start_lprob), chunk.nstates, stream) <
        chunk.nstates) {
        imm_error("could not read start_lprob");
        goto err;
    }

    if (fread(&chunk.end_state, sizeof(chunk.end_state), 1, stream) < 1) {
        imm_error("could not read end_state");
        goto err;
    }

    state_tbl->nstates = chunk.nstates;
    state_tbl->min_seq = chunk.min_seq;
    state_tbl->max_seq = chunk.max_seq;
    state_tbl->start_lprob = chunk.start_lprob;
    state_tbl->end_state = chunk.end_state;

    return state_tbl;

err:
    free_c(state_tbl);

    if (chunk.min_seq)
        free_c(chunk.min_seq);

    if (chunk.max_seq)
        free_c(chunk.max_seq);

    if (chunk.start_lprob)
        free_c(chunk.start_lprob);

    return NULL;
}
