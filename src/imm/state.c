#include "imm.h"
#include "src/imm/ascii.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

struct imm_state
{
    const char*           name;
    const struct imm_abc* abc;

    imm_state_lprob_t   lprob;
    imm_state_min_seq_t min_seq;
    imm_state_max_seq_t max_seq;
    void*               impl;
};

struct imm_state* imm_state_create(const char* name, const struct imm_abc* abc,
                                   struct imm_state_funcs funcs, void* impl)
{
    if (imm_abc_length(abc) == 0) {
        imm_error("empty alphabet");
        return NULL;
    }

    if (!is_std_ascii(name, strlen(name))) {
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

void* imm_state_get_impl(struct imm_state* state) { return state->impl; }

const void* imm_state_get_impl_c(const struct imm_state* state) { return state->impl; }

const char* imm_state_get_name(const struct imm_state* s) { return s->name; }

const struct imm_abc* imm_state_get_abc(const struct imm_state* s) { return s->abc; }

double imm_state_lprob(const struct imm_state* state, const char* seq, int seq_len)
{
    for (int i = 0; i < seq_len; ++i) {
        if (!imm_abc_has_symbol(imm_state_get_abc(state), seq[i])) {
            imm_error("alphabet does not have symbol %c", seq[i]);
            return NAN;
        }
    }
    return state->lprob(state, seq, seq_len);
}

void imm_state_destroy(struct imm_state* state)
{
    if (!state)
        return;

    free((char*)state->name);
    state->abc = NULL;
    state->lprob = NULL;
    state->min_seq = NULL;
    state->max_seq = NULL;
    free(state);
}

int imm_state_min_seq(const struct imm_state* state) { return state->min_seq(state); }

int imm_state_max_seq(const struct imm_state* state) { return state->max_seq(state); }

struct imm_state* imm_state_cast(void* state)
{
    struct type
    {
        struct imm_state* state;
    }* t = state;
    return t->state;
}

const struct imm_state* imm_state_cast_c(const void* state)
{
    const struct type_c
    {
        const struct imm_state* state;
    }* t = state;
    return t->state;
}
