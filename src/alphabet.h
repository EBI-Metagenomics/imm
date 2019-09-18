#ifndef _NHMM_ALPHABET_H
#define _NHMM_ALPHABET_H

#include "nhmm.h"
#include "sds.h"

struct nhmm_alphabet
{
    sds abc;
    int idx[128];
};

inline static int alphabet_has_symbol(const struct nhmm_alphabet *a, char id)
{
    return a->idx[id] != -1;
}

inline static int alphabet_symbol_idx(const struct nhmm_alphabet *a, char id)
{
    return a->idx[id];
}

inline static char alphabet_symbol_id(const struct nhmm_alphabet *a, int idx)
{
    return a->abc[idx];
}

#endif
