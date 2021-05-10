#ifndef IMM_CODON_H
#define IMM_CODON_H

#include "imm/compiler.h"
#include "imm/error.h"
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

static inline int imm_codon_set(struct imm_codon *codon,
                                struct imm_triplet triplet)
{
    struct imm_abc const *abc = imm_nuclt_super(codon->nuclt);

    if (imm_unlikely(!(__imm_abc_symbol_type(abc, triplet.a) != IMM_SYM_NULL &&
                       __imm_abc_symbol_type(abc, triplet.b) != IMM_SYM_NULL &&
                       __imm_abc_symbol_type(abc, triplet.c) != IMM_SYM_NULL)))
    {
        imm_log_error("invalid triplet");
        return IMM_ILLEGALARG;
    }

    codon->a = __imm_abc_symbol_idx(abc, triplet.a);
    codon->b = __imm_abc_symbol_idx(abc, triplet.b);
    codon->c = __imm_abc_symbol_idx(abc, triplet.c);
    return IMM_SUCCESS;
}

static inline struct imm_codon imm_codon_init(struct imm_nuclt const *nuclt)
{
    return (struct imm_codon){
        nuclt, .a = imm_abc_any_symbol_idx(imm_nuclt_super(nuclt)),
        .b = imm_abc_any_symbol_idx(imm_nuclt_super(nuclt)),
        .c = imm_abc_any_symbol_idx(imm_nuclt_super(nuclt))};
}

#endif
