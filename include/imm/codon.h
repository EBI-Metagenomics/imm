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
    struct imm_triplet triplet;
};

static inline struct imm_triplet const *
imm_codon_get(struct imm_codon const *codon)
{
    return &codon->triplet;
}

static inline int imm_codon_set(struct imm_codon *codon,
                                struct imm_triplet triplet)
{
    struct imm_abc const *abc = imm_nuclt_super(codon->nuclt);

    if (imm_unlikely(!(imm_abc_symbol_type(abc, triplet.a) != IMM_SYM_NULL &&
                       imm_abc_symbol_type(abc, triplet.b) != IMM_SYM_NULL &&
                       imm_abc_symbol_type(abc, triplet.c) != IMM_SYM_NULL)))
    {
        imm_log_error("invalid triplet");
        return IMM_ILLEGALARG;
    }

    codon->triplet = triplet;
    return IMM_SUCCESS;
}

#define IMM_CODON_INIT(nuclt)                                                  \
    (struct imm_codon)                                                         \
    {                                                                          \
        (nuclt), IMM_TRIPLET_INIT(imm_abc_any_symbol(imm_nuclt_super(nuclt)),  \
                                  imm_abc_any_symbol(imm_nuclt_super(nuclt)),  \
                                  imm_abc_any_symbol(imm_nuclt_super(nuclt)))  \
    }

static inline void imm_codon_init(struct imm_codon *codon,
                                  struct imm_nuclt const *nuclt)
{
    codon->nuclt = nuclt;
    codon->triplet.a = imm_abc_any_symbol(imm_nuclt_super(nuclt));
    codon->triplet.b = imm_abc_any_symbol(imm_nuclt_super(nuclt));
    codon->triplet.c = imm_abc_any_symbol(imm_nuclt_super(nuclt));
    codon->triplet.pad = '\0';
}

#endif
