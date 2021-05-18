#include "trans_table.h"
#include "args.h"
#include "containers/stack.h"
#include "imm/trans.h"
#include "support.h"
#include "trans.h"
#include <limits.h>

unsigned trans_table_idx(struct trans_table *trans_tbl, unsigned src_idx,
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

void trans_table_init(struct trans_table *tbl)
{
    tbl->ntrans = UINT_MAX;
    tbl->trans = NULL;
    tbl->offset = NULL;
}

void trans_table_reset(struct trans_table *tbl, struct dp_args const *args)
{
    tbl->ntrans = args->ntransitions;
    tbl->offset =
        xrealloc(tbl->offset,
                 sizeof(*tbl->offset) * trans_table_offset_size(args->nstates));
    tbl->offset[0] = 0;

    if (tbl->ntrans > 0)
        tbl->trans = xrealloc(tbl->trans, sizeof(*tbl->trans) * tbl->ntrans);
    else
        tbl->trans = NULL;

    for (unsigned i = 0; i < args->nstates; ++i)
    {
        struct iter it = stack_iter(&args->states[i]->trans.incoming);
        struct trans *trans = NULL;
        unsigned j = 0;
        iter_for_each_entry(trans, &it, incoming)
        {
            tbl->trans[tbl->offset[i] + j].score = trans->lprob;
            tbl->trans[tbl->offset[i] + j].src = trans->pair.idx.src;
            ++j;
        }
        tbl->offset[i + 1] = (imm_trans_idx_t)(tbl->offset[i] + j);
    }
}

void trans_table_del(struct trans_table const *tbl)
{
    free(tbl->trans);
    free(tbl->offset);
}
