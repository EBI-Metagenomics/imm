#ifndef _NHMM_STATE_STATE_H
#define _NHMM_STATE_STATE_H

#include "nhmm/alphabet.h"
#include "nhmm/state.h"

#include "sds.h"

typedef void (*state_destroy)(struct nhmm_state *state);
typedef double (*state_emission)(struct nhmm_state *state, const char *x, size_t xlen);

struct nhmm_state
{
    sds name;
    const struct nhmm_alphabet *a;
    int end;

    state_destroy destroy;
    state_emission emission;
    void *impl;
};

#endif
