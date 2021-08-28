#include "dp/trans_table.h"
#include "cco/stack.h"
#include "dp/dp.h"
#include "imm/trans.h"
#include "support.h"
#include "xmem.h"
#include <limits.h>

unsigned trans_table_idx(struct imm_dp_trans_table *trans_tbl, unsigned src_idx,
                         unsigned dst_idx)
{
    for (unsigned i = 0; i < trans_table_ntrans(trans_tbl, dst_idx); ++i)
    {
        if (trans_table_source_state(trans_tbl, dst_idx, i) == src_idx)
            return trans_tbl->offset[dst_idx] + i;
    }
    error(IMM_ILLEGALARG, "transition not found");
    return IMM_TRANS_NULL_IDX;
}

void trans_table_init(struct imm_dp_trans_table *tbl)
{
    tbl->ntrans = UINT_MAX;
    tbl->trans = NULL;
    tbl->offset = NULL;
}

void trans_table_reset(struct imm_dp_trans_table *tbl,
                       struct dp_args const *args)
{
    IMM_BUG(args->nstates == 0);
    tbl->ntrans = args->ntrans;
    tbl->offset = xrealloc(tbl->offset, sizeof(*tbl->offset) *
                                            trans_table_offsize(args->nstates));
    tbl->offset[0] = 0;

    tbl->trans = xrealloc(tbl->trans, sizeof(*tbl->trans) * tbl->ntrans);

    for (unsigned i = 0; i < args->nstates; ++i)
    {
        struct cco_iter it = cco_stack_iter(&args->states[i]->trans.incoming);
        struct imm_trans *trans = NULL;
        unsigned j = 0;
        cco_iter_for_each_entry(trans, &it, incoming)
        {
            tbl->trans[tbl->offset[i] + j].score = trans->lprob;
            tbl->trans[tbl->offset[i] + j].src = trans->pair.idx.src;
            ++j;
        }
        tbl->offset[i + 1] = (imm_trans_idx_t)(tbl->offset[i] + j);
    }
}

void trans_table_del(struct imm_dp_trans_table const *tbl)
{
    free(tbl->trans);
    free(tbl->offset);
}
