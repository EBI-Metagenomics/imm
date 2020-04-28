#include "imm/table_state.h"
#include "free.h"
#include "imm/seq_table.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include "min.h"
#include "state.h"
#include <stdlib.h>

struct imm_table_state
{
    struct imm_state const*     super;
    struct imm_seq_table const* table;
};

static uint8_t  type_id(struct imm_state const* state);
static double   lprob(struct imm_state const* state, struct imm_seq const* seq);
static unsigned min_seq(struct imm_state const* state);
static unsigned max_seq(struct imm_state const* state);
static int      write(struct imm_state const* state, struct imm_io const* io, FILE* stream);
static void     destroy(struct imm_state const* state);

static struct imm_state_vtable const vtable = {type_id, lprob, min_seq, max_seq, write, destroy};

struct imm_table_state const* imm_table_state_create(char const*                 name,
                                                     struct imm_seq_table const* table)
{
    struct imm_table_state* state = malloc(sizeof(struct imm_table_state));
    state->table = imm_seq_table_clone(table);

    state->super = imm_state_create(name, imm_seq_table_get_abc(table), vtable, state);
    return state;
}

void imm_table_state_destroy(struct imm_table_state const* state)
{
    state->super->vtable.destroy(state->super);
}

struct imm_state const* imm_table_state_super(struct imm_table_state const* state)
{
    return state->super;
}

struct imm_table_state const* imm_table_state_derived(struct imm_state const* state)
{
    if (imm_state_type_id(state) != IMM_TABLE_STATE_TYPE_ID) {
        imm_error("could not cast to table_state");
        return NULL;
    }
    return __imm_state_derived(state);
}

static uint8_t type_id(struct imm_state const* state) { return IMM_TABLE_STATE_TYPE_ID; }

static double lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    struct imm_table_state const* s = __imm_state_derived(state);
    return imm_seq_table_lprob(s->table, seq);
}

static unsigned min_seq(struct imm_state const* state)
{
    struct imm_table_state const* s = __imm_state_derived(state);
    return imm_seq_table_min_seq(s->table);
}

static unsigned max_seq(struct imm_state const* state)
{
    struct imm_table_state const* s = __imm_state_derived(state);
    return imm_seq_table_max_seq(s->table);
}

static int write(struct imm_state const* state, struct imm_io const* io, FILE* stream)
{
    imm_die("table_state_write not implemented");
    return 0;
}

static void destroy(struct imm_state const* state)
{
    struct imm_table_state const* s = __imm_state_derived(state);
    imm_seq_table_destroy(s->table);
    free_c(s);
    __imm_state_destroy(state);
}
