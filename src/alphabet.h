#ifndef _NHMM_ALPHABET_H
#define _NHMM_ALPHABET_H

#include "nhmm.h"
#include "sds.h"

struct nhmm_alphabet
{
    sds symbols;
    int symbol_idx[128];
};

inline static int alphabet_has_symbol(const struct nhmm_alphabet *alphabet,
                                      char symbol_id)
{
    return alphabet->symbol_idx[symbol_id] != -1;
}

inline static int alphabet_symbol_idx(const struct nhmm_alphabet *alphabet,
                                      char symbol_id)
{
    return alphabet->symbol_idx[symbol_id];
}

inline static char alphabet_symbol_id(const struct nhmm_alphabet *alphabet,
                                      int symbol_idx)
{
    return alphabet->symbols[symbol_idx];
}

#endif
