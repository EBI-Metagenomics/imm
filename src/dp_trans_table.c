#include "dp_trans_table.h"
#include "free.h"
#include "imm/lprob.h"
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

struct dp_trans_chunk
{
    uint32_t  ntrans;
    double*   score;
    uint32_t* source_state;
    uint32_t  offset_size;
    uint32_t* offset;
};

static uint32_t create_incoming_transitions(struct list_head*           incoming_trans,
                                            struct mstate const* const* mstates,
                                            uint32_t                    nstates,
                                            struct state_idx const*     state_idx);

static inline uint32_t score_size(uint32_t ntrans) { return ntrans; }

static inline uint32_t source_state_size(uint32_t ntrans) { return ntrans; }

static inline uint32_t offset_size(uint32_t nstates) { return nstates + 1; }

struct dp_trans_table const* dp_trans_table_create(struct mstate const* const* mstates, uint32_t nstates,
                                       struct state_idx* state_idx)
{
    struct list_head incoming_trans[nstates];
    for (uint32_t i = 0; i < nstates; ++i)
        INIT_LIST_HEAD(incoming_trans + i);

    uint32_t ntrans = create_incoming_transitions(incoming_trans, mstates, nstates, state_idx);

    struct dp_trans_table* trans_tbl = malloc(sizeof(*trans_tbl));
    trans_tbl->ntrans = ntrans;
    trans_tbl->offset = malloc(sizeof(*trans_tbl->offset) * offset_size(nstates));
    trans_tbl->offset[0] = 0;

    if (ntrans > 0) {
        trans_tbl->score = malloc(sizeof(*trans_tbl->score) * score_size(ntrans));
        trans_tbl->source_state =
            malloc(sizeof(*trans_tbl->source_state) * source_state_size(ntrans));
    } else {
        trans_tbl->score = NULL;
        trans_tbl->source_state = NULL;
    }

    for (uint32_t i = 0; i < nstates; ++i) {
        uint32_t               j = 0;
        struct incoming_trans* it = NULL;
        list_for_each_entry (it, incoming_trans + i, list_entry) {

            trans_tbl->score[trans_tbl->offset[i] + j] = it->score;
            trans_tbl->source_state[trans_tbl->offset[i] + j] = it->source_state;
            ++j;
        }
        trans_tbl->offset[i + 1] = trans_tbl->offset[i] + j;
    }

    for (uint32_t i = 0; i < nstates; ++i) {
        struct incoming_trans* it = NULL;
        struct incoming_trans* tmp = NULL;
        list_for_each_entry_safe (it, tmp, incoming_trans + i, list_entry) {
            free_c(it);
        }
    }

    return trans_tbl;
}

static uint32_t create_incoming_transitions(struct list_head*           incoming_trans,
                                            struct mstate const* const* mstates,
                                            uint32_t                    nstates,
                                            struct state_idx const*     state_idx)
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

            struct incoming_trans* it = malloc(sizeof(struct incoming_trans));
            it->score = lprob;
            it->source_state = src;
            list_add_tail(&it->list_entry, incoming_trans + dst);
            ++ntrans;
        }
    }
    return ntrans;
}

void dp_trans_table_destroy(struct dp_trans_table const* trans_tbl)
{
    free_c(trans_tbl->score);
    free_c(trans_tbl->source_state);
    free_c(trans_tbl->offset);
    free_c(trans_tbl);
}

int dp_trans_table_write(struct dp_trans_table const* trans, uint32_t nstates, FILE* stream)
{
    struct dp_trans_chunk chunk = {.ntrans = trans->ntrans,
                                   .score = trans->score,
                                   .source_state = trans->source_state,
                                   .offset_size = nstates + 1,
                                   .offset = trans->offset};

    if (fwrite(&chunk.ntrans, sizeof(chunk.ntrans), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.score, sizeof(*chunk.score), score_size(trans->ntrans), stream) <
        score_size(trans->ntrans))
        return 1;

    if (fwrite(chunk.source_state, sizeof(*chunk.source_state),
               source_state_size(trans->ntrans), stream) < source_state_size(trans->ntrans))
        return 1;

    if (fwrite(&chunk.offset_size, sizeof(chunk.offset_size), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.offset, sizeof(*chunk.offset), offset_size(nstates), stream) <
        offset_size(nstates))
        return 1;

    return 0;
}
