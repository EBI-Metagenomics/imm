#include "imm/state_table.h"
#include "assume.h"
#include "dp.h"
#include "error.h"
#include "imm/dp_args.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include "reallocf.h"
#include "span.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>

void imm_state_table_init(struct imm_state_table *x)
{
    x->nstates = IMM_NSTATES_NULL;
    x->ids = NULL;
    x->start.state = IMM_STATE_NULL_IDX;
    x->start.lprob = IMM_LPROB_NAN;
    x->end_state_idx = IMM_STATE_NULL_IDX;
    x->span = NULL;
}

void imm_state_table_del(struct imm_state_table const *x)
{
    if (x)
    {
        free(x->ids);
        free(x->span);
    }
}

int imm_state_table_reset(struct imm_state_table *x,
                          struct imm_dp_args const *args)
{
    x->nstates = args->nstates;
    x->ids = imm_reallocf(x->ids, sizeof(*x->ids) * args->nstates);
    x->span = imm_reallocf(x->span, sizeof(*x->span) * args->nstates);

    if (args->nstates > 0)
    {
        if (!x->ids || !x->span) return error(IMM_NOMEM);
    }

    for (unsigned i = 0; i < args->nstates; ++i)
    {
        x->ids[i] = (imm_state_id_t)imm_state_id(args->states[i]);
        x->span[i] = SPAN(imm_state_span(args->states[i]).min,
                          imm_state_span(args->states[i]).max);
    }

    x->start.lprob = args->start.lprob;
    x->start.state = (imm_state_idx_t)args->start.state->idx;
    x->end_state_idx = args->end_state->idx;
    return IMM_OK;
}

unsigned imm_state_table_idx(struct imm_state_table const *x, unsigned state_id)
{
    for (unsigned idx = 0; idx < x->nstates; ++idx)
    {
        if (x->ids[idx] == state_id) return idx;
    }
    return UINT_MAX;
}

unsigned imm_state_table_id(struct imm_state_table const *x, unsigned idx)
{
    return x->ids[idx];
}

struct span imm_state_table_span(struct imm_state_table const *x,
                                 unsigned state)
{
    struct span span = x->span[state];
    assert(span.min <= span.max && !(span.min == 0 && span.max > 0));
    assume(span.min <= span.max && !(span.min == 0 && span.max > 0));
    assume(span.max <= IMM_STATE_MAX_SEQLEN);
    return span;
}

struct imm_range imm_state_table_range(struct imm_state_table const *x,
                                       unsigned state)
{
    struct span span = imm_state_table_span(x, state);
    return imm_range_init(span.min, span.max + 1);
}

void imm_state_table_dump(struct imm_state_table const *tbl)
{
    for (unsigned i = 0; i < tbl->nstates; ++i)
    {
        printf("%u", i);
        if (i + 1 < tbl->nstates) putc('\t', stdout);
    }
    putc('\n', stdout);

    for (unsigned i = 0; i < tbl->nstates; ++i)
    {
        printf("%u", tbl->ids[i]);
        if (i + 1 < tbl->nstates) putc('\t', stdout);
    }
    putc('\n', stdout);
}
