#include "dp/state_table.h"
#include "dp/dp.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include "support.h"
#include <limits.h>

void state_table_init(struct imm_dp_state_table *tbl)
{
    tbl->nstates = IMM_NSTATES_NULL;
    tbl->ids = NULL;
    tbl->start.state = IMM_STATE_NULL_IDX;
    tbl->start.lprob = IMM_LPROB_NAN;
    tbl->end_state_idx = IMM_STATE_NULL_IDX;
    tbl->span = NULL;
}

void state_table_reset(struct imm_dp_state_table *tbl,
                       struct dp_args const *args)
{
    tbl->nstates = args->nstates;
    tbl->ids = xrealloc(tbl->ids, sizeof(*tbl->ids) * args->nstates);
    tbl->span = xrealloc(tbl->span, sizeof(*tbl->span) * args->nstates);

    for (unsigned i = 0; i < args->nstates; ++i)
    {
        tbl->ids[i] = (imm_state_id_t)imm_state_id(args->states[i]);
        tbl->span[i] = SPAN(imm_state_span(args->states[i]).min,
                            imm_state_span(args->states[i]).max);
    }

    tbl->start.lprob = args->start.lprob;
    tbl->start.state = (imm_state_idx_t)args->start.state->idx;
    tbl->end_state_idx = args->end_state->idx;
}

void state_table_del(struct imm_dp_state_table const *tbl)
{
    free(tbl->ids);
    free(tbl->span);
}

#ifndef NDEBUG
void state_table_dump(struct imm_dp_state_table const *tbl)
{
    for (unsigned i = 0; i < tbl->nstates; ++i)
    {
        printf("%u", i);
        if (i + 1 < tbl->nstates)
            putc('\t', stdout);
    }
    putc('\n', stdout);

    for (unsigned i = 0; i < tbl->nstates; ++i)
    {
        printf("%u", tbl->ids[i]);
        if (i + 1 < tbl->nstates)
            putc('\t', stdout);
    }
    putc('\n', stdout);
}
#endif
