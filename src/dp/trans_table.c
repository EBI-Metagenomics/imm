#include "trans_table.h"
#include "args.h"
#include "common/common.h"
#include "containers/stack.h"
#include "imm/error.h"
#include "imm/trans.h"
#include "trans.h"

static inline unsigned offset_size(unsigned nstates) { return nstates + 1; }

unsigned trans_table_idx(struct trans_table *trans_tbl, unsigned src_idx,
                         unsigned dst_idx)
{
    for (unsigned i = 0; i < trans_table_ntrans(trans_tbl, dst_idx); ++i)
    {
        if (trans_table_source_state(trans_tbl, dst_idx, i) == src_idx)
            return trans_tbl->offset[dst_idx] + i;
    }
    xerror(IMM_ILLEGALARG, "transition not found");
    return IMM_NULL_TRANS;
}

void trans_table_init(struct trans_table *tbl, struct dp_args const *args)
{
    tbl->ntrans = args->ntransitions;
    tbl->offset = xmalloc(sizeof(*tbl->offset) * offset_size(args->nstates));
    tbl->offset[0] = 0;

    if (tbl->ntrans > 0)
        tbl->trans = xmalloc(sizeof(*tbl->trans) * tbl->ntrans);
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

void trans_table_deinit(struct trans_table const *tbl)
{
    free(tbl->trans);
    free(tbl->offset);
}
