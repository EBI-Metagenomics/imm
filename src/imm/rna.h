#ifndef IMM_RNA_H
#define IMM_RNA_H

#include "imm/export.h"
#include "imm/nuclt.h"
#include "imm/rc.h"

#define IMM_RNA_ANY_SYMBOL 'X'
#define IMM_RNA_SYMBOLS "ACGU"
#define IMM_RNA_SIZE 4

struct imm_rna
{
    struct imm_nuclt super;
};

IMM_API extern struct imm_rna const imm_rna_default;

IMM_API enum imm_rc imm_rna_init(struct imm_rna *rna,
                                 char const symbols[IMM_RNA_SIZE],
                                 char any_symbol);

IMM_API enum imm_rc imm_rna_write(struct imm_rna const *rna, FILE *file);

IMM_API enum imm_rc imm_rna_read(struct imm_rna *rna, FILE *file);

static inline struct imm_nuclt const *imm_rna_super(struct imm_rna const *rna)
{
    return &rna->super;
}

static inline enum imm_abc_typeid imm_rna_typeid(struct imm_rna const *rna)
{
    return rna->super.super.vtable.typeid;
}

#endif
