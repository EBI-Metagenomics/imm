#ifndef IMM_RNA_H
#define IMM_RNA_H

#include "imm/export.h"
#include "imm/nuclt.h"

#define IMM_RNA_ANY_SYMBOL 'X'
#define IMM_RNA_SYMBOLS "ACGU"
#define IMM_RNA_NSYMBOLS 4

struct imm_rna
{
    struct imm_nuclt super;
};

extern struct imm_rna const imm_rna_default;

IMM_API int imm_rna_init(struct imm_rna *rna,
                         char const symbols[IMM_RNA_NSYMBOLS], char any_symbol);

static inline struct imm_nuclt const *imm_rna_super(struct imm_rna const *rna)
{
    return &rna->super;
}

#endif
