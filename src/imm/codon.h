#ifndef IMM_CODON_H
#define IMM_CODON_H

#include "imm/nuclt.h"
#include <assert.h>

struct imm_codon
{
    struct imm_nuclt const *nuclt;
    union
    {
        struct __attribute__((__packed__))
        {
            unsigned a;
            unsigned b;
            unsigned c;
        };
        unsigned idx[3];
    };
};

static inline void imm_codon_set(struct imm_codon *codon, unsigned id_a,
                                 unsigned id_b, unsigned id_c)
{
    struct imm_abc const *abc = imm_nuclt_super(codon->nuclt);

    assert(!(__imm_abc_symbol_type(abc, id_a) == IMM_SYM_NULL ||
             __imm_abc_symbol_type(abc, id_b) == IMM_SYM_NULL ||
             __imm_abc_symbol_type(abc, id_c) == IMM_SYM_NULL));

    codon->a = __imm_abc_symbol_idx(abc, id_a);
    codon->b = __imm_abc_symbol_idx(abc, id_b);
    codon->c = __imm_abc_symbol_idx(abc, id_c);
}

static inline char imm_codon_asym(struct imm_codon const *codon)
{
    return imm_abc_symbols(&codon->nuclt->super)[codon->a];
}

static inline char imm_codon_bsym(struct imm_codon const *codon)
{
    return imm_abc_symbols(&codon->nuclt->super)[codon->b];
}

static inline char imm_codon_csym(struct imm_codon const *codon)
{
    return imm_abc_symbols(&codon->nuclt->super)[codon->c];
}

static inline struct imm_codon imm_codon(struct imm_nuclt const *nuclt,
                                         unsigned id_a, unsigned id_b,
                                         unsigned id_c)
{
    struct imm_codon codon = {.nuclt = nuclt};
    imm_codon_set(&codon, id_a, id_b, id_c);
    return codon;
}

static inline struct imm_codon imm_codon_any(struct imm_nuclt const *nuclt)
{
    unsigned any = imm_abc_any_symbol_id(&nuclt->super);
    return imm_codon(nuclt, any, any, any);
}

static inline struct imm_codon
imm_codon_from_symbols(struct imm_nuclt const *nuclt, char a, char b, char c)
{
    return imm_codon(nuclt, imm_sym_id(a), imm_sym_id(b), imm_sym_id(c));
}

#define IMM_CODON(n, x) imm_codon_from_symbols(n, x[0], x[1], x[2])

#endif
