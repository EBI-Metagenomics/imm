#ifndef IMM_CODON_H
#define IMM_CODON_H

#include "imm/compiler.h"
#include "imm/log.h"
#include "imm/nuclt.h"
#include "imm/sym.h"
#include "imm/triplet.h"

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

static inline void imm_codon_set(struct imm_codon *codon,
                                 struct imm_triplet triplet)
{
    struct imm_abc const *abc = imm_nuclt_super(codon->nuclt);

    IMM_BUG(__imm_abc_symbol_type(abc, triplet.a) != IMM_SYM_NULL ||
            __imm_abc_symbol_type(abc, triplet.b) != IMM_SYM_NULL ||
            __imm_abc_symbol_type(abc, triplet.c) != IMM_SYM_NULL);

    codon->a = __imm_abc_symbol_idx(abc, triplet.a);
    codon->b = __imm_abc_symbol_idx(abc, triplet.b);
    codon->c = __imm_abc_symbol_idx(abc, triplet.c);
}

#define IMM_CODON(abc, a, b, c) imm_codon((abc), IMM_TRIPLET((a), (b), (c)))

static inline struct imm_codon imm_codon(struct imm_nuclt const *nuclt,
                                         struct imm_triplet triplet)
{
    struct imm_codon codon;
    codon.nuclt = nuclt;
    imm_codon_set(&codon, triplet);
    return codon;
}

#endif
