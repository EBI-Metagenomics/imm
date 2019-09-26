#ifndef NHMM_STATE_STATE_H
#define NHMM_STATE_STATE_H

#include "nhmm.h"
#include "rapidstring.h"

typedef void (*state_destroy)(struct nhmm_state *state);
typedef double (*state_emiss_lprob)(const struct nhmm_state *state, const char *seq,
                                    size_t seq_len);
typedef int (*state_normalize)(struct nhmm_state *state);

struct nhmm_state
{
    rapidstring name;
    const struct nhmm_alphabet *alphabet;
    bool end_state;

    state_destroy destroy;
    state_emiss_lprob emiss_lprob;
    state_normalize normalize;
    void *impl;
};

#endif
