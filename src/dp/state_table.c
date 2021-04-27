#include "dp/state_table.h"
#include "common/common.h"
#include "imm/state.h"

struct chunk
{
    uint16_t nstates;
    uint8_t *min_seq;
    uint8_t *max_seq;
    uint16_t start_state;
    imm_float start_lprob;
    uint16_t end_state;
};

struct state_table const *state_table_new(struct dp_args const *args)
{
    struct state_table *table = xmalloc(sizeof(*table));

    table->nstates = args->nstates;
    table->min_seq = xmalloc(sizeof(*table->min_seq) * args->nstates);
    table->max_seq = xmalloc(sizeof(*table->max_seq) * args->nstates);

    for (unsigned i = 0; i < args->nstates; ++i)
    {
        table->min_seq[i] = imm_state_min_seq(args->states[i]);
        table->max_seq[i] = imm_state_max_seq(args->states[i]);
    }

    table->start.lprob = args->start.lprob;
    table->start.state = args->start.state->idx;
    table->end_state = args->end_state->idx;

    return table;
}

void state_table_del(struct state_table const *tbl)
{
    free(tbl->min_seq);
    free(tbl->max_seq);
    free((void *)tbl);
}

#if 0
struct state_table *dp_state_table_read(FILE *stream)
{
    struct chunk chunk = {.nstates = 0,
                                         .min_seq = NULL,
                                         .max_seq = NULL,
                                         .start_state = 0,
                                         .start_lprob = 0.,
                                         .end_state = 0};

    struct state_table *table = xmalloc(sizeof(*table));

    if (fread(&chunk.nstates, sizeof(chunk.nstates), 1, stream) < 1)
    {
        error("could not read nstates");
        goto err;
    }

    chunk.min_seq = xmalloc(sizeof(*chunk.min_seq) * chunk.nstates);
    chunk.max_seq = xmalloc(sizeof(*chunk.max_seq) * chunk.nstates);

    if (fread(chunk.min_seq, sizeof(*chunk.min_seq), chunk.nstates, stream) <
        chunk.nstates)
    {
        error("could not read min_seq");
        goto err;
    }

    if (fread(chunk.max_seq, sizeof(*chunk.max_seq), chunk.nstates, stream) <
        chunk.nstates)
    {
        error("could not read max_seq");
        goto err;
    }

    if (fread(&chunk.start_state, sizeof(chunk.start_state), 1, stream) < 1)
    {
        error("could not read start_state");
        goto err;
    }

    if (fread(&chunk.start_lprob, sizeof(chunk.start_lprob), 1, stream) < 1)
    {
        error("could not read start_lprob");
        goto err;
    }

    if (fread(&chunk.end_state, sizeof(chunk.end_state), 1, stream) < 1)
    {
        error("could not read end_state");
        goto err;
    }

    table->nstates = chunk.nstates;
    table->min_seq = chunk.min_seq;
    table->max_seq = chunk.max_seq;
    table->start_state = chunk.start_state;
    table->start_lprob = chunk.start_lprob;
    table->end_state = chunk.end_state;

    return table;

err:
    free(table);
    free_if(chunk.min_seq);
    free_if(chunk.max_seq);
    return NULL;
}

int dp_state_table_write(struct state_table const *state_tbl, FILE *stream)
{
    struct chunk chunk = {.nstates = state_tbl->nstates,
                                         .min_seq = state_tbl->min_seq,
                                         .max_seq = state_tbl->max_seq,
                                         .start_state = state_tbl->start_state,
                                         .start_lprob = state_tbl->start_lprob,
                                         .end_state = state_tbl->end_state};

    if (fwrite(&chunk.nstates, sizeof(chunk.nstates), 1, stream) < 1)
    {
        error("could not write nstates");
        return 1;
    }

    if (fwrite(chunk.min_seq, sizeof(*chunk.min_seq), state_tbl->nstates,
               stream) < state_tbl->nstates)
    {
        error("could not write min_seq");
        return 1;
    }

    if (fwrite(chunk.max_seq, sizeof(*chunk.max_seq), state_tbl->nstates,
               stream) < state_tbl->nstates)
    {
        error("could not write max_seq");
        return 1;
    }

    if (fwrite(&chunk.start_state, sizeof(chunk.start_state), 1, stream) < 1)
    {
        error("could not write start_state");
        return 1;
    }

    if (fwrite(&chunk.start_lprob, sizeof(chunk.start_lprob), 1, stream) < 1)
    {
        error("could not write start_lprob");
        return 1;
    }

    if (fwrite(&chunk.end_state, sizeof(chunk.end_state), 1, stream) < 1)
    {
        error("could not write end_state");
        return 1;
    }

    return 0;
}
#endif
