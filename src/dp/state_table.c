#include "dp/state_table.h"
#include "common/common.h"
#include "dp/args.h"

void state_table_init(struct state_table *tbl, struct dp_args const *args)
{
    tbl->nstates = args->nstates;
    tbl->ids = xmalloc(sizeof(*tbl->ids) * args->nstates);
    tbl->seqlen = xmalloc(sizeof(*tbl->seqlen) * args->nstates);

    for (unsigned i = 0; i < args->nstates; ++i)
        tbl->ids[i] = (__imm_state_id_t)imm_state_id(args->states[i]);

    for (unsigned i = 0; i < args->nstates; ++i)
    {
        tbl->seqlen[i].min =
            (__imm_state_seqlen_t)imm_state_min_seqlen(args->states[i]);
        tbl->seqlen[i].max =
            (__imm_state_seqlen_t)imm_state_max_seqlen(args->states[i]);
    }

    tbl->start.lprob = args->start.lprob;
    tbl->start.state = (__imm_state_idx_t)args->start.state->idx;
    tbl->end_state = args->end_state->idx;
}

void state_table_deinit(struct state_table const *tbl)
{
    free(tbl->ids);
    free(tbl->seqlen);
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
