#include "imm/trans_table.h"
#include "error.h"
#include "imm/cco.h"
#include "imm/dp_args.h"
#include "imm/list.h"
#include "imm/trans.h"
#include "reallocf.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>

void imm_trans_table_init(struct imm_trans_table *x)
{
    x->ntrans = UINT_MAX;
    x->trans = NULL;
    x->offset = NULL;
}

int imm_trans_table_reset(struct imm_trans_table *x,
                          struct imm_dp_args const *args)
{
    assert(args->nstates > 0);
    x->ntrans = args->ntrans;

    unsigned offsize = imm_trans_table_offsize(args->nstates);
    x->offset = reallocf(x->offset, sizeof(*x->offset) * offsize);
    if (!x->offset && offsize > 0) return error(IMM_NOMEM);

    x->offset[0] = 0;

    x->trans = reallocf(x->trans, sizeof(*x->trans) * x->ntrans);
    if (!x->trans && x->ntrans > 0)
    {
        x->offset = NULL;
        free(x->offset);
        return error(IMM_NOMEM);
    }

    for (unsigned i = 0; i < args->nstates; ++i)
    {
        struct imm_trans *trans = NULL;
        unsigned j = 0;
        imm_list_for_each_entry(trans, &args->states[i]->trans.incoming,
                                incoming)
        {
            x->trans[x->offset[i] + j].score = trans->lprob;
            x->trans[x->offset[i] + j].src = trans->pair.idx.src;
            ++j;
        }
        x->offset[i + 1] = (imm_trans_idx_t)(x->offset[i] + j);
    }
    return IMM_OK;
}

unsigned imm_trans_table_idx(struct imm_trans_table const *x, unsigned src,
                             unsigned dst)
{
    for (unsigned i = 0; i < imm_trans_table_ntrans(x, dst); ++i)
    {
        if (imm_trans_table_source_state(x, dst, i) == src)
            return x->offset[dst] + i;
    }
    error(IMM_TRANSITION_NOT_FOUND);
    return IMM_TRANS_NULL_IDX;
}

void imm_trans_table_change(struct imm_trans_table *x, unsigned trans,
                            imm_float score)
{
    x->trans[trans].score = score;
}

void imm_trans_table_del(struct imm_trans_table const *x)
{
    if (x)
    {
        free(x->trans);
        free(x->offset);
    }
}

unsigned imm_trans_table_ntrans(struct imm_trans_table const *tbl, unsigned dst)
{
    return (unsigned)(tbl->offset[dst + 1] - tbl->offset[dst]);
}

imm_float imm_trans_table_score(struct imm_trans_table const *tbl, unsigned dst,
                                unsigned trans)
{
    return tbl->trans[tbl->offset[dst] + trans].score;
}

unsigned imm_trans_table_source_state(struct imm_trans_table const *tbl,
                                      unsigned dst, unsigned trans)
{
    return tbl->trans[tbl->offset[dst] + trans].src;
}

unsigned imm_trans_table_offsize(unsigned nstates) { return nstates + 1; }
