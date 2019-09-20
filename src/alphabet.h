#ifndef _NHMM_ALPHABET_H
#define _NHMM_ALPHABET_H

#include "nhmm.h"
#include "rapidstring.h"

#define NHMM_SYMBOL_ID_FIRST 0
#define NHMM_SYMBOL_ID_LAST 127

struct nhmm_alphabet
{
    rapidstring symbols;
    int symbol_idx[NHMM_SYMBOL_ID_LAST + 1];
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
    return rs_data_c(&alphabet->symbols)[symbol_idx];
}

#endif
