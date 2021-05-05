#ifndef IMM_DNA_H
#define IMM_DNA_H

#include "imm/export.h"
#include "imm/nuclt.h"

#define IMM_DNA_ANY_SYMBOL 'X'
#define IMM_DNA_SYMBOLS "ACGT"
#define IMM_DNA_NSYMBOLS 4

struct imm_dna
{
    struct imm_nuclt super;
};

extern struct imm_dna const imm_dna_default;

IMM_API int imm_dna_init(struct imm_dna *dna,
                         char const symbols[IMM_DNA_NSYMBOLS], char any_symbol);

static inline struct imm_nuclt const *imm_dna_super(struct imm_dna const *dna)
{
    return &dna->super;
}

#endif
