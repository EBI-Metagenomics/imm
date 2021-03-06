#include "imm/imm.h"
#include "std.h"
#include <stdlib.h>

struct imm_table_state
{
    struct imm_state*           super;
    struct imm_seq_table const* table;
};

static void      destroy(struct imm_state const* state);
static imm_float lprob(struct imm_state const* state, struct imm_seq const* seq);
static uint8_t   max_seq(struct imm_state const* state);
static uint8_t   min_seq(struct imm_state const* state);
static uint8_t   type_id(struct imm_state const* state);

static struct imm_state_vtable const __vtable = {destroy, lprob, max_seq, min_seq, type_id};

struct imm_table_state* imm_table_state_create(uint16_t id, char const* name, struct imm_seq_table const* table)
{
    struct imm_table_state* state = xmalloc(sizeof(*state));
    state->table = imm_seq_table_clone(table);

    state->super = imm_state_create(id, name, imm_seq_table_abc(table), __vtable, state);
    return state;
}

struct imm_table_state* imm_table_state_derived(struct imm_state const* state)
{
    if (imm_state_type_id(state) != IMM_TABLE_STATE_TYPE_ID) {
        error("could not cast to table_state");
        return NULL;
    }
    return __imm_state_derived(state);
}

void imm_table_state_destroy(struct imm_table_state const* state) { state->super->vtable.destroy(state->super); }

struct imm_state* imm_table_state_read(FILE* stream, struct imm_abc const* abc)
{
    BUG("imm_table_state_read not implemented");
    return NULL;
}

struct imm_state* imm_table_state_super(struct imm_table_state const* state) { return state->super; }

static void destroy(struct imm_state const* state)
{
    struct imm_table_state const* s = __imm_state_derived(state);
    imm_seq_table_destroy(s->table);
    free((void*)s);
    __imm_state_destroy(state);
}

int imm_table_state_write(struct imm_state const* state, struct imm_profile const* prof, FILE* stream)
{
    error("table_state write is not implemented yet");
    return 0;
}

static imm_float lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    struct imm_table_state const* s = __imm_state_derived(state);
    return imm_seq_table_lprob(s->table, seq);
}

static uint8_t max_seq(struct imm_state const* state)
{
    struct imm_table_state const* s = __imm_state_derived(state);
    return imm_seq_table_max_seq(s->table);
}

static uint8_t min_seq(struct imm_state const* state)
{
    struct imm_table_state const* s = __imm_state_derived(state);
    return imm_seq_table_min_seq(s->table);
}

static uint8_t type_id(struct imm_state const* state) { return IMM_TABLE_STATE_TYPE_ID; }
