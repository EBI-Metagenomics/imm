#include "dp_trans.h"
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
    unsigned         source_state;
    double           score;
    struct list_head list_entry;
};

static unsigned create_incoming_transitions(struct list_head*           incoming_trans,
                                            struct mstate const* const* mstates,
                                            unsigned                    nstates,
                                            struct state_idx const*     state_idx);

struct dp_trans const* dp_trans_create(struct mstate const* const* mstates, unsigned nstates,
                                       struct state_idx* state_idx)
{
    struct list_head incoming_trans[nstates];
    for (unsigned i = 0; i < nstates; ++i)
        INIT_LIST_HEAD(incoming_trans + i);

    unsigned ntrans = create_incoming_transitions(incoming_trans, mstates, nstates, state_idx);

    struct dp_trans* trans_tbl = malloc(sizeof(*trans_tbl));
    trans_tbl->offset = malloc(sizeof(*trans_tbl->offset) * dp_trans_offset_size(nstates));
    trans_tbl->offset[0] = 0;

    if (ntrans > 0) {
        trans_tbl->score = malloc(sizeof(*trans_tbl->score) * dp_trans_score_size(ntrans));
        trans_tbl->source_state =
            malloc(sizeof(*trans_tbl->source_state) * dp_trans_source_state_size(ntrans));
    } else {
        trans_tbl->score = NULL;
        trans_tbl->source_state = NULL;
    }

    for (unsigned i = 0; i < nstates; ++i) {
        unsigned               j = 0;
        struct incoming_trans* it = NULL;
        list_for_each_entry (it, incoming_trans + i, list_entry) {

            trans_tbl->score[trans_tbl->offset[i] + j] = it->score;
            trans_tbl->source_state[trans_tbl->offset[i] + j] = it->source_state;
            ++j;
        }
        trans_tbl->offset[i + 1] = trans_tbl->offset[i] + j;
    }

    for (unsigned i = 0; i < nstates; ++i) {
        struct incoming_trans* it = NULL;
        struct incoming_trans* tmp = NULL;
        list_for_each_entry_safe (it, tmp, incoming_trans + i, list_entry) {
            free_c(it);
        }
    }

    return trans_tbl;
}

static unsigned create_incoming_transitions(struct list_head*           incoming_trans,
                                            struct mstate const* const* mstates,
                                            unsigned                    nstates,
                                            struct state_idx const*     state_idx)
{
    unsigned ntrans = 0;
    for (unsigned i = 0; i < nstates; ++i) {

        struct imm_state const*    src_state = mstate_get_state(mstates[i]);
        unsigned                   src = state_idx_find(state_idx, src_state);
        struct mtrans_table const* table = mstate_get_mtrans_table(mstates[i]);

        unsigned long iter = 0;
        mtrans_table_for_each (iter, table) {
            if (!mtrans_table_exist(table, iter))
                continue;
            struct mtrans const* mtrans = mtrans_table_get(table, iter);
            double               lprob = mtrans_get_lprob(mtrans);

            if (imm_lprob_is_zero(lprob))
                continue;

            struct imm_state const* dst_state = mtrans_get_state(mtrans);
            unsigned                dst = state_idx_find(state_idx, dst_state);

            struct incoming_trans* it = malloc(sizeof(struct incoming_trans));
            it->score = lprob;
            it->source_state = src;
            list_add_tail(&it->list_entry, incoming_trans + dst);
            ++ntrans;
        }
    }
    return ntrans;
}

void dp_trans_destroy(struct dp_trans const* trans_tbl)
{
    free_c(trans_tbl->score);
    free_c(trans_tbl->source_state);
    free_c(trans_tbl->offset);
    free_c(trans_tbl);
}
