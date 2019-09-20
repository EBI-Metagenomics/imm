#ifndef _NHMM_HMM_H
#define _NHMM_HMM_H

#include "nhmm/hmm.h"
#include "nhmm/alphabet.h"

struct nhmm_hmm
{
    const struct nhmm_alphabet *alphabet;

    struct nhmm_state *states;
    struct counter* state_id_counter;
};

#endif
