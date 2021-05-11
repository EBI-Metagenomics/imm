#ifndef IMM_CODON_H
#define IMM_CODON_H

#include "imm/compiler.h"
#include "imm/log.h"
#include "imm/nuclt.h"
#include "imm/sym.h"

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

static inline void imm_codon_set(struct imm_codon *codon, unsigned a,
                                 unsigned b, unsigned c)
{
    struct imm_abc const *abc = imm_nuclt_super(codon->nuclt);

    IMM_BUG(__imm_abc_symbol_type(abc, a) == IMM_SYM_NULL ||
            __imm_abc_symbol_type(abc, b) == IMM_SYM_NULL ||
            __imm_abc_symbol_type(abc, c) == IMM_SYM_NULL);

    codon->a = __imm_abc_symbol_idx(abc, a);
    codon->b = __imm_abc_symbol_idx(abc, b);
    codon->c = __imm_abc_symbol_idx(abc, c);
}

static inline struct imm_codon imm_codon_from_id(struct imm_nuclt const *nuclt,
                                                 unsigned a, unsigned b,
                                                 unsigned c)
{
    struct imm_codon codon = {.nuclt = nuclt};
    imm_codon_set(&codon, a, b, c);
    return codon;
}

static inline struct imm_codon
imm_codon_from_char(struct imm_nuclt const *nuclt, char a, char b, char c)
{
    return imm_codon_from_id(nuclt, imm_sym_id(a), imm_sym_id(b),
                             imm_sym_id(c));
}

#define imm_codon(n, a, b, c)                                                  \
    _Generic((a), char                                                         \
             : imm_codon_from_char, int                                        \
             : imm_codon_from_char, unsigned                                   \
             : imm_codon_from_id)(n, a, b, c)

#endif
