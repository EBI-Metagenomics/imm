#include "imm/table_state.h"
#include "free.h"
#include "imm/seq_table.h"
#include "imm/state.h"
#include "imm/state_factory.h"
#include "min.h"
#include <stdlib.h>

struct imm_table_state
{
    struct imm_state const*     interface;
    struct imm_seq_table const* table;
};

static double   table_state_lprob(struct imm_state const* state, struct imm_seq const* seq);
static unsigned table_state_min_seq(struct imm_state const* state);
static unsigned table_state_max_seq(struct imm_state const* state);
static int      table_state_write(struct imm_state const* state, FILE* stream);

struct imm_table_state* imm_table_state_create(char const*                 name,
                                               struct imm_seq_table const* table)
{
    struct imm_table_state* state = malloc(sizeof(struct imm_table_state));
    state->table = imm_seq_table_clone(table);

    struct imm_state_funcs funcs = {table_state_lprob, table_state_min_seq,
                                    table_state_max_seq, table_state_write};
    state->interface = imm_state_create(name, imm_seq_table_get_abc(table), funcs,
                                        IMM_TABLE_STATE_TYPE_ID, state);
    return state;
}

void imm_table_state_destroy(struct imm_table_state const* state)
{
    imm_state_destroy(state->interface);
    imm_seq_table_destroy(state->table);
    free_c(state);
}

static double table_state_lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    struct imm_table_state const* s = imm_state_get_impl_c(state);
    return imm_seq_table_lprob(s->table, seq);
}

static unsigned table_state_min_seq(struct imm_state const* state)
{
    struct imm_table_state const* s = imm_state_get_impl_c(state);
    return imm_seq_table_min_seq(s->table);
}

static unsigned table_state_max_seq(struct imm_state const* state)
{
    struct imm_table_state const* s = imm_state_get_impl_c(state);
    return imm_seq_table_max_seq(s->table);
}

static int table_state_write(struct imm_state const* state, FILE* stream)
{
    imm_die("table_state_write not implemented");
    return 0;
}
