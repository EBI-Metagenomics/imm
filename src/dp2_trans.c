#include "dp2_trans.h"
#include "free.h"
#include "mstate.h"
#include "mtrans.h"
#include "mtrans_table.h"
#include "state_idx.h"
#include <stdlib.h>

struct dp2_trans const* dp2_trans_create(struct mstate const* const* mstates, unsigned nstates,
                                         struct state_idx* state_idx)
{
    struct dp2_trans* trans_tbl = malloc(sizeof(struct dp2_trans));
    trans_tbl->offset = malloc(sizeof(unsigned) * (nstates + 1));
    trans_tbl->offset[0] = 0;

    unsigned ntrans = mtrans_table_size(mstate_get_mtrans_table(mstates[0]));
    for (unsigned i = 1; i < nstates; ++i) {
        trans_tbl->offset[i] = ntrans;
        ntrans += mtrans_table_size(mstate_get_mtrans_table(mstates[i]));
    }
    /* Used for (offset[i+1] - offset[i+1]) calculation. */
    trans_tbl->offset[nstates] = ntrans;

    if (ntrans > 0) {
        trans_tbl->cost = malloc(sizeof(double) * ntrans);
        trans_tbl->target_state = malloc(sizeof(unsigned) * ntrans);
    } else {
        trans_tbl->cost = NULL;
        trans_tbl->target_state = NULL;
    }

    for (unsigned i = 0; i < nstates; ++i) {
        struct mtrans_table const* tbl = mstate_get_mtrans_table(mstates[i]);
        unsigned long              iter = 0;
        unsigned                   j = 0;
        mtrans_table_for_each (iter, tbl) {
            if (!mtrans_table_exist(tbl, iter))
                continue;

            struct mtrans* mtrans = mtrans_table_get(tbl, iter);
            trans_tbl->cost[trans_tbl->offset[i] + j] = mtrans_get_lprob(mtrans);
            trans_tbl->target_state[trans_tbl->offset[i] + j] =
                state_idx_find(state_idx, mtrans_get_state(mtrans));
            ++j;
        }
    }

    return trans_tbl;
}

void dp2_trans_destroy(struct dp2_trans const* trans_tbl)
{
    imm_free(trans_tbl->cost);
    imm_free(trans_tbl->target_state);
    imm_free(trans_tbl->offset);
    imm_free(trans_tbl);
}
