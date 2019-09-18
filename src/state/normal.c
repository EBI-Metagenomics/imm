#include "state/normal.h"

#include <stdlib.h>

void normal_state_create(struct nhmm_state *state, double *emission)
{
    struct normal_state *s = malloc(sizeof(struct nhmm_state));
    s->emission = emission;
    state->impl = s;
}

double normal_state_emission(struct nhmm_state *state, const char* x, size_t xlen)
{
    return 0.0;
    /* if (xlen == 1) */
    /* { */
    /*     return -NHMM_INF; */
    /* } */

    /* return ns->emission[s->a->char2int[x[0]]]; */
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
