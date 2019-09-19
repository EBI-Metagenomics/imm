#ifndef _NHMM_STATE_STATE_H
#define _NHMM_STATE_STATE_H

#include "nhmm/alphabet.h"
#include "nhmm/state.h"

#include "sds.h"

typedef void (*state_destroy)(struct nhmm_state *state);
typedef double (*state_emission_lprob)(struct nhmm_state *state, const char *seq,
                                       size_t seq_len);

struct nhmm_state
{
    sds name;
    const struct nhmm_alphabet *alphabet;
    bool end_state;

    state_destroy destroy;
    state_emission_lprob emission_lprob;
    void *impl;
};

#endif