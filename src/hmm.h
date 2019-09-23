#ifndef _NHMM_HMM_H
#define _NHMM_HMM_H

#include "nhmm/hmm.h"
#include "nhmm/alphabet.h"

struct tbl_state;

struct nhmm_hmm
{
    const struct nhmm_alphabet *alphabet;

    /* Maps `state_id` to `state`. */
    struct tbl_state *tbl_states;
    /* `state_id`s pool: starts with 0, then 1, and so on. */
    struct counter* state_id_counter;
};

#endif
