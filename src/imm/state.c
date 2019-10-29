#include "imm.h"
#include "src/imm/ascii.h"
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

struct imm_state* imm_state_create(char const* name, struct imm_abc const* abc,
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

void imm_state_destroy(struct imm_state* state)
{
    if (!state) {
        imm_error("state should not be NULL");
        return;
    }

    free((char*)state->name);
    state->abc = NULL;
    state->lprob = NULL;
    state->min_seq = NULL;
    state->max_seq = NULL;
    free(state);
}

char const* imm_state_get_name(struct imm_state const* s) { return s->name; }

struct imm_abc const* imm_state_get_abc(struct imm_state const* s) { return s->abc; }

double imm_state_lprob(struct imm_state const* state, char const* seq, int seq_len)
{
    for (int i = 0; i < seq_len; ++i) {
        if (!imm_abc_has_symbol(imm_state_get_abc(state), seq[i])) {
            imm_error("alphabet does not have symbol %c", seq[i]);
            return imm_lprob_invalid();
        }
    }
    return state->lprob(state, seq, seq_len);
}

int imm_state_min_seq(struct imm_state const* state) { return state->min_seq(state); }

int imm_state_max_seq(struct imm_state const* state) { return state->max_seq(state); }

struct imm_state* imm_state_cast(void* state)
{
    struct type
    {
        struct imm_state* state;
    }* t = state;
    return t->state;
}

struct imm_state const* imm_state_cast_c(void const* state)
{
    struct type_c
    {
        struct imm_state const* state;
    } const* t = state;
    return t->state;
}

void* imm_state_get_impl(struct imm_state* state) { return state->impl; }

void const* imm_state_get_impl_c(struct imm_state const* state) { return state->impl; }
