#include "dp/state_table.h"
#include "dp/args.h"
#include "support.h"

void state_table_init(struct state_table *tbl, struct dp_args const *args)
{
    tbl->nstates = args->nstates;
    tbl->ids = xmalloc(sizeof(*tbl->ids) * args->nstates);
    tbl->span = xmalloc(sizeof(*tbl->span) * args->nstates);

    for (unsigned i = 0; i < args->nstates; ++i)
    {
        tbl->ids[i] = (imm_state_id_t)imm_state_id(args->states[i]);
        tbl->span[i] = SPAN(imm_state_span(args->states[i]).min,
                            imm_state_span(args->states[i]).max);
    }

    tbl->start.lprob = args->start.lprob;
    tbl->start.state = (imm_state_idx_t)args->start.state->idx;
    tbl->end_state = args->end_state->idx;
}

void state_table_deinit(struct state_table const *tbl)
{
    free(tbl->ids);
    free(tbl->span);
}

#ifndef NDEBUG
void state_table_dump(struct state_table const *tbl)
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
