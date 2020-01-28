#include "free.h"
#include "imm/imm.h"
#include "min.h"
#include <math.h>
#include <stdlib.h>

struct imm_table_state
{
    struct imm_state const*     interface;
    struct imm_seq_table const* table;
};

static double   table_state_lprob(struct imm_state const* state, struct imm_seq seq);
static unsigned table_state_min_seq(struct imm_state const* state);
static unsigned table_state_max_seq(struct imm_state const* state);

struct imm_table_state* imm_table_state_create(char const*                 name,
                                               struct imm_seq_table const* table)
{
    struct imm_table_state* state = malloc(sizeof(struct imm_table_state));
    state->table = table;

    struct imm_state_funcs funcs = {table_state_lprob, table_state_min_seq,
                                    table_state_max_seq};
    state->interface = imm_state_create(name, imm_seq_table_get_abc(table), funcs, state);
    return state;
}

void imm_table_state_destroy(struct imm_table_state const* state)
{
    if (!state) {
        imm_error("state should not be NULL");
        return;
    }

    imm_state_destroy(state->interface);
    imm_seq_table_destroy(state->table);
    free_c(state);
}

static double table_state_lprob(struct imm_state const* state, struct imm_seq const seq)
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
