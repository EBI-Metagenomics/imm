#ifndef IMM_AMINO_H
#define IMM_AMINO_H

#include "imm/export.h"

#define IMM_AMINO_ANY_SYMBOL 'X'
#define IMM_AMINO_SYMBOLS "ACDEFGHIKLMNPQRSTVWY"
#define IMM_AMINO_NSYMBOLS ((unsigned)(sizeof(IMM_AMINO_SYMBOLS) - 1))

struct imm_abc;

struct imm_amino
{
    struct imm_abc const *super;
};

IMM_API struct imm_amino const *imm_amino_new(void);

IMM_API void imm_amino_del(struct imm_amino const *amino);

static inline struct imm_abc const *
imm_amino_super(struct imm_amino const *amino)
{
    return amino->super;
}

#endif
