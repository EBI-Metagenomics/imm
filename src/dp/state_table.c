#include "dp/state_table.h"
#include "common/common.h"
#include "dp/args.h"

void state_table_init(struct state_table *tbl, struct dp_args const *args)
{
    tbl->nstates = args->nstates;
    tbl->seqlen = xmalloc(sizeof(*tbl->seqlen) * args->nstates);

    for (unsigned i = 0; i < args->nstates; ++i)
    {
        tbl->seqlen[i].min = (uint8_t)imm_state_min_seqlen(args->states[i]);
        tbl->seqlen[i].max = (uint8_t)imm_state_max_seqlen(args->states[i]);
    }

    tbl->start.lprob = args->start.lprob;
    tbl->start.state = args->start.state->idx;
    tbl->end_state = args->end_state->idx;
}

void state_table_deinit(struct state_table const *tbl)
{
    free(tbl->seqlen);
    free((void *)tbl);
}
