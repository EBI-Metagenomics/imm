#ifndef _NHMM_STATE_H
#define _NHMM_STATE_H

#include "sds.h"
#include "nhmm/alphabet.h"

struct nhmm_state
{
    sds name;
    const struct nhmm_alphabet *a;
    void(*destroy)(struct nhmm_state*);
};

#endif
