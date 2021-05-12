#ifndef IMM_CODON_H
#define IMM_CODON_H

#include "imm/nuclt.h"

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

    IMM_BUG(__imm_abc_symbol_type(abc, id_a) == IMM_SYM_NULL ||
            __imm_abc_symbol_type(abc, id_b) == IMM_SYM_NULL ||
            __imm_abc_symbol_type(abc, id_c) == IMM_SYM_NULL);

    codon->a = __imm_abc_symbol_idx(abc, id_a);
    codon->b = __imm_abc_symbol_idx(abc, id_b);
    codon->c = __imm_abc_symbol_idx(abc, id_c);
}

static inline struct imm_codon
__imm_codon_from_id(struct imm_nuclt const *nuclt, unsigned a, unsigned b,
                    unsigned c)
{
    struct imm_codon codon = {.nuclt = nuclt};
    imm_codon_set(&codon, a, b, c);
    return codon;
}

static inline struct imm_codon
__imm_codon_from_char(struct imm_nuclt const *nuclt, char a, char b, char c)
{
    return __imm_codon_from_id(nuclt, imm_sym_id(a), imm_sym_id(b),
                               imm_sym_id(c));
}

#define imm_codon(n, a, b, c)                                                  \
    _Generic((a), char                                                         \
             : __imm_codon_from_char, int                                      \
             : __imm_codon_from_char, unsigned                                 \
             : __imm_codon_from_id)(n, a, b, c)

#endif
