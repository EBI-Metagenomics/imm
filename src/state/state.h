#ifndef _NHMM_STATE_STATE_H
#define _NHMM_STATE_STATE_H

#include "nhmm/alphabet.h"

#include "sds.h"

struct nhmm_state
{
    sds name;
    const struct nhmm_alphabet *a;
    void (*destroy)(struct nhmm_state *);
    void (*emission)(const char* x, size_t xlen);
    void *data;
};

#endif
