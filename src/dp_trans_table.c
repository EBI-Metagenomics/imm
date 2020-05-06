#include "dp_trans_table.h"
#include "free.h"
#include "imm/lprob.h"
#include "io.h"
#include "list.h"
#include "mstate.h"
#include "mtrans.h"
#include "mtrans_table.h"
#include "state_idx.h"
#include <stdlib.h>

struct incoming_trans
{
    uint32_t         source_state;
    double           score;
    struct list_head list_entry;
};

struct dp_trans_table_chunk
{
    uint32_t  ntrans;
    double*   score;
    uint32_t* source_state;
    uint32_t  offset_size;
    uint32_t* offset;
};

static uint32_t        create_incoming_transitions(struct list_head*           incoming_trans,
                                                   struct mstate const* const* mstates, uint32_t nstates,
                                                   struct state_idx const* state_idx);
static inline uint32_t offset_size(uint32_t nstates) { return nstates + 1; }
static inline uint32_t score_size(uint32_t ntrans) { return ntrans; }
static inline uint32_t source_state_size(uint32_t ntrans) { return ntrans; }

struct dp_trans_table const* dp_trans_table_create(struct mstate const* const* mstates,
                                                   uint32_t nstates, struct state_idx* state_idx)
{
    struct list_head incoming_trans[nstates];
    for (uint32_t i = 0; i < nstates; ++i)
        INIT_LIST_HEAD(incoming_trans + i);

    uint32_t ntrans = create_incoming_transitions(incoming_trans, mstates, nstates, state_idx);

    struct dp_trans_table* tbl = malloc(sizeof(*tbl));
    tbl->ntrans = ntrans;
    tbl->offset = malloc(sizeof(*tbl->offset) * offset_size(nstates));
    tbl->offset[0] = 0;

    if (ntrans > 0) {
        tbl->score = malloc(sizeof(*tbl->score) * score_size(ntrans));
        tbl->source_state = malloc(sizeof(*tbl->source_state) * source_state_size(ntrans));
    } else {
        tbl->score = NULL;
        tbl->source_state = NULL;
    }

    for (uint32_t i = 0; i < nstates; ++i) {
        uint32_t               j = 0;
        struct incoming_trans* it = NULL;
        list_for_each_entry (it, incoming_trans + i, list_entry) {

            tbl->score[tbl->offset[i] + j] = it->score;
            tbl->source_state[tbl->offset[i] + j] = it->source_state;
            ++j;
        }
        tbl->offset[i + 1] = tbl->offset[i] + j;
    }

    for (uint32_t i = 0; i < nstates; ++i) {
        struct incoming_trans* it = NULL;
        struct incoming_trans* tmp = NULL;
        list_for_each_entry_safe (it, tmp, incoming_trans + i, list_entry)
            free_c(it);
    }

    return tbl;
}

void dp_trans_table_destroy(struct dp_trans_table const* trans_tbl)
{
    free_c(trans_tbl->score);
    free_c(trans_tbl->source_state);
    free_c(trans_tbl->offset);
    free_c(trans_tbl);
}

struct dp_trans_table const* dp_trans_table_read(FILE* stream)
{
    struct dp_trans_table_chunk chunk = {
        .ntrans = 0, .score = NULL, .source_state = NULL, .offset_size = 0, .offset = NULL};

    struct dp_trans_table* trans_tbl = malloc(sizeof(*trans_tbl));

    if (fread(&chunk.ntrans, sizeof(chunk.ntrans), 1, stream) < 1) {
        imm_error("could not read ntransitions");
        goto err;
    }

    chunk.score = malloc(sizeof(*chunk.score) * score_size(chunk.ntrans));

    if (fread(chunk.score, sizeof(*chunk.score), score_size(chunk.ntrans), stream) <
        score_size(chunk.ntrans)) {
        imm_error("could not read score");
        goto err;
    }

    chunk.source_state = malloc(sizeof(*chunk.source_state) * source_state_size(chunk.ntrans));

    if (fread(chunk.source_state, sizeof(*chunk.source_state), source_state_size(chunk.ntrans),
              stream) < source_state_size(chunk.ntrans)) {
        imm_error("could not read source_state");
        goto err;
    }

    if (fread(&chunk.offset_size, sizeof(chunk.offset_size), 1, stream) < 1) {
        imm_error("could not read offset_size");
        goto err;
    }

    chunk.offset = malloc(sizeof(*chunk.offset) * chunk.offset_size);

    if (fread(chunk.offset, sizeof(*chunk.offset), chunk.offset_size, stream) < chunk.offset_size) {
        imm_error("could not read offset");
        goto err;
    }

    trans_tbl->ntrans = chunk.ntrans;
    trans_tbl->score = chunk.score;
    trans_tbl->source_state = chunk.source_state;
    trans_tbl->offset = chunk.offset;

    return trans_tbl;

err:
    if (chunk.score)
        free_c(chunk.score);

    if (chunk.source_state)
        free_c(chunk.source_state);

    if (chunk.offset)
        free_c(chunk.offset);

    free_c(trans_tbl);

    return NULL;
}

int dp_trans_table_write(struct dp_trans_table const* trans, uint32_t nstates, FILE* stream)
{
    struct dp_trans_table_chunk chunk = {.ntrans = trans->ntrans,
                                         .score = trans->score,
                                         .source_state = trans->source_state,
                                         .offset_size = nstates + 1,
                                         .offset = trans->offset};

    if (fwrite(&chunk.ntrans, sizeof(chunk.ntrans), 1, stream) < 1) {
        imm_error("could not write ntransitions");
        return 1;
    }

    if (fwrite(chunk.score, sizeof(*chunk.score), score_size(trans->ntrans), stream) <
        score_size(trans->ntrans)) {
        imm_error("could not write score");
        return 1;
    }

    if (fwrite(chunk.source_state, sizeof(*chunk.source_state), source_state_size(trans->ntrans),
               stream) < source_state_size(trans->ntrans)) {
        imm_error("could not write source_state");
        return 1;
    }

    if (fwrite(&chunk.offset_size, sizeof(chunk.offset_size), 1, stream) < 1) {
        imm_error("could not write offset_size");
        return 1;
    }

    if (fwrite(chunk.offset, sizeof(*chunk.offset), offset_size(nstates), stream) <
        offset_size(nstates)) {
        imm_error("could not write offset");
        return 1;
    }

    return 0;
}

static uint32_t create_incoming_transitions(struct list_head*           incoming_trans,
                                            struct mstate const* const* mstates, uint32_t nstates,
                                            struct state_idx const* state_idx)
{
    uint32_t ntrans = 0;
    for (uint32_t i = 0; i < nstates; ++i) {

        struct imm_state const*    src_state = mstate_get_state(mstates[i]);
        uint32_t                   src = state_idx_find(state_idx, src_state);
        struct mtrans_table const* table = mstate_get_mtrans_table(mstates[i]);

        unsigned long iter = 0;
        mtrans_table_for_each (iter, table) {
            if (!mtrans_table_exist(table, iter))
                continue;
            struct mtrans const* mtrans = mtrans_table_get(table, iter);
            double               lprob = mtrans_get_lprob(mtrans);

            struct imm_state const* dst_state = mtrans_get_state(mtrans);
            uint32_t                dst = state_idx_find(state_idx, dst_state);

            struct incoming_trans* it = malloc(sizeof(*it));
            it->score = lprob;
            it->source_state = src;
            list_add_tail(&it->list_entry, incoming_trans + dst);
            ++ntrans;
        }
    }
    return ntrans;
}
