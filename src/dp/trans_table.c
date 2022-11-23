#include "dp/trans_table.h"
#include "dp/dp.h"
#include "error.h"
#include "imm/cco.h"
#include "imm/trans.h"
#include "reallocf.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>

unsigned imm_trans_table_idx(struct imm_dp_trans_table *trans_tbl,
                             unsigned src_idx, unsigned dst_idx)
{
    for (unsigned i = 0; i < trans_table_ntrans(trans_tbl, dst_idx); ++i)
    {
        if (trans_table_source_state(trans_tbl, dst_idx, i) == src_idx)
            return trans_tbl->offset[dst_idx] + i;
    }
    error(IMM_TRANSITION_NOT_FOUND);
    return IMM_TRANS_NULL_IDX;
}

void imm_trans_table_init(struct imm_dp_trans_table *tbl)
{
    tbl->ntrans = UINT_MAX;
    tbl->trans = NULL;
    tbl->offset = NULL;
}

enum imm_rc imm_trans_table_reset(struct imm_dp_trans_table *tbl,
                                  struct dp_args const *args)
{
    assert(args->nstates > 0);
    tbl->ntrans = args->ntrans;

    unsigned offsize = trans_table_offsize(args->nstates);
    tbl->offset = reallocf(tbl->offset, sizeof(*tbl->offset) * offsize);
    if (!tbl->offset && offsize > 0) return error(IMM_NOMEM);

    tbl->offset[0] = 0;

    tbl->trans = reallocf(tbl->trans, sizeof(*tbl->trans) * tbl->ntrans);
    if (!tbl->trans && tbl->ntrans > 0)
    {
        tbl->offset = NULL;
        free(tbl->offset);
        return error(IMM_NOMEM);
    }

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
    return IMM_OK;
}

void imm_trans_table_del(struct imm_dp_trans_table const *tbl)
{
    free(tbl->trans);
    free(tbl->offset);
}
