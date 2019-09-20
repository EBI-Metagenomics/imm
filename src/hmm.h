#ifndef _NHMM_HMM_H
#define _NHMM_HMM_H

#include "nhmm/hmm.h"
#include "nhmm/alphabet.h"

struct tbl_state;

struct nhmm_hmm
{
    const struct nhmm_alphabet *alphabet;

    struct tbl_state *tbl_states;
    struct counter* state_id_counter;
};

#endif
