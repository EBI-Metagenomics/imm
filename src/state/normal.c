#include "alphabet.h"
#include "state/normal.h"

#include <math.h>
#include <stdlib.h>

void normal_state_create(struct nhmm_state *state, double *emission)
{
    struct normal_state *s = malloc(sizeof(struct nhmm_state));
    s->emission = emission;
    state->impl = s;
}

double normal_state_emission(struct nhmm_state *state, const char* x, size_t xlen)
{
    struct normal_state *s = state->impl;
    if (xlen == 1)
    {
        if (alphabet_has_symbol(state->a, x[0]))
            return s->emission[alphabet_symbol_idx(state->a, x[0])];
    }
    return -INFINITY;
}

void normal_state_destroy(struct nhmm_state *state)
{
    if (!state)
        return;

    if (!state->impl)
        return;

    struct normal_state *s = state->impl;
    if (s->emission)
        free(s->emission);

    free(state->impl);
}
