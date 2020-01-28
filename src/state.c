#include "ascii.h"
#include "bug.h"
#include "free.h"
#include "imm/imm.h"
#include <stdlib.h>
#include <string.h>

struct imm_state
{
    char const*           name;
    struct imm_abc const* abc;

    imm_state_lprob_t   lprob;
    imm_state_min_seq_t min_seq;
    imm_state_max_seq_t max_seq;
    void*               impl;
};

struct imm_state const* imm_state_create(char const* name, struct imm_abc const* abc,
                                         struct imm_state_funcs funcs, void* impl)
{
    if (imm_abc_length(abc) == 0) {
        imm_error("empty alphabet");
        return NULL;
    }

    if (!ascii_is_std(name, strlen(name))) {
        imm_error("name must be a string of non-extended ASCII characters");
        return NULL;
    }

    struct imm_state* s = malloc(sizeof(struct imm_state));
    s->name = strdup(name);
    s->abc = abc;
    s->lprob = funcs.lprob;
    s->min_seq = funcs.min_seq;
    s->max_seq = funcs.max_seq;
    s->impl = impl;
    return s;
}

void imm_state_destroy(struct imm_state const* state)
{
    free_c(state->name);
    free_c(state);
}

char const* imm_state_get_name(struct imm_state const* s) { return s->name; }

struct imm_abc const* imm_state_get_abc(struct imm_state const* s) { return s->abc; }

double imm_state_lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    BUG(state->abc != imm_seq_get_abc(seq));
    return state->lprob(state, seq);
}

unsigned imm_state_min_seq(struct imm_state const* state) { return state->min_seq(state); }

unsigned imm_state_max_seq(struct imm_state const* state) { return state->max_seq(state); }

struct imm_state const* imm_state_cast_c(void const* state)
{
    struct type_c
    {
        struct imm_state const* state;
    } const* t = state;
    return t->state;
}

void const* imm_state_get_impl_c(struct imm_state const* state) { return state->impl; }
