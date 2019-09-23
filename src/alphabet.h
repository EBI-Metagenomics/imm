#ifndef NHMM_ALPHABET_H
#define NHMM_ALPHABET_H

#include "nhmm.h"
#include "rapidstring.h"
#include "report.h"

#define NHMM_SYMBOL_ID_MIN 0
#define NHMM_SYMBOL_ID_MAX 127

struct nhmm_alphabet
{
    rapidstring symbols;
    int symbol_idx[NHMM_SYMBOL_ID_MAX + 1];
};

inline static int check_symbol_id_range(char symbol_id)
{
    if (symbol_id < NHMM_SYMBOL_ID_MIN || symbol_id > NHMM_SYMBOL_ID_MAX) {
        error("symbols must be non-extended ASCII characters");
        return 1;
    }
    return 0;
}

inline static int alphabet_has_symbol(const struct nhmm_alphabet *alphabet,
                                      char symbol_id)
{
    check_symbol_id_range(symbol_id);
    return alphabet->symbol_idx[(size_t)symbol_id] != -1;
}

inline static int alphabet_symbol_idx(const struct nhmm_alphabet *alphabet,
                                      char symbol_id)
{
    if (check_symbol_id_range(symbol_id))
        return -1;
    return alphabet->symbol_idx[(size_t)symbol_id];
}

inline static char alphabet_symbol_id(const struct nhmm_alphabet *alphabet,
                                      int symbol_idx)
{
    return rs_data_c(&alphabet->symbols)[symbol_idx];
}

#endif
