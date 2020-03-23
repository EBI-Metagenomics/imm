#include "dp/trans_table.h"
#include "free.h"
#include "mstate.h"
#include "mtrans.h"
#include "mtrans_table.h"
#include "state_idx.h"
#include <stdlib.h>

struct dp_trans_table
{
    double*   cost;
    unsigned* source_state;
    unsigned* offset;
};

struct dp_trans_table const* dp_trans_table_create(struct mstate const* const* mstates,
                                                   unsigned                    nstates,
                                                   struct state_idx*           state_idx)
{
    struct dp_trans_table* t = malloc(sizeof(struct dp_trans_table));
    t->offset = malloc(sizeof(unsigned) * nstates);
    t->offset[0] = 0;

    unsigned ntrans = mtrans_table_size(mstate_get_mtrans_table(mstates[0]));
    for (unsigned i = 1; i < nstates; ++i) {
        t->offset[i] = ntrans;
        ntrans += mtrans_table_size(mstate_get_mtrans_table(mstates[i]));
    }

    t->source_state = malloc(sizeof(unsigned) * ntrans);

    for (unsigned i = 0; i < nstates; ++i) {
        struct mtrans_table const* tbl = mstate_get_mtrans_table(mstates[i]);
        unsigned long              iter = 0;
        unsigned                   j = 0;
        mtrans_table_for_each (iter, tbl) {
            if (mtrans_table_exist(tbl, iter)) {
                struct mtrans* mtrans = mtrans_table_get(tbl, iter);
                t->cost[t->offset[i] + j] = mtrans_get_lprob(mtrans);
                t->source_state[t->offset[i] + j] =
                    state_idx_find(state_idx, mtrans_get_state(mtrans));
                ++j;
            }
        }
    }

    return t;
}

/* double dp_trans_table_cost(struct dp_trans_table const* transition, unsigned source_state)
 */

void dp_trans_table_destroy(struct dp_trans_table const* transition)
{
    imm_free(transition->cost);
    imm_free(transition->source_state);
    imm_free(transition);
}
