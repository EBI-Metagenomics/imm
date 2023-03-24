#ifndef IMM_NUCLT_H
#define IMM_NUCLT_H

#include "imm/abc.h"
#include "imm/export.h"
#include "imm/rc.h"

#define IMM_NUCLT_ANY_SYMBOL 'X'
#define IMM_NUCLT_SYMBOLS "ACGT"
#define IMM_NUCLT_SIZE 4

struct imm_nuclt
{
    struct imm_abc super;
};

IMM_API extern struct imm_nuclt const imm_nuclt_empty;

IMM_API int imm_nuclt_init(struct imm_nuclt *nuclt,
                           char const symbols[IMM_NUCLT_SIZE], char any_symbol);

static inline unsigned imm_nuclt_size(struct imm_nuclt const *nuclt)
{
    (void)nuclt;
    return IMM_NUCLT_SIZE;
}

static inline struct imm_abc const *
imm_nuclt_super(struct imm_nuclt const *nuclt)
{
    return &nuclt->super;
}

static inline enum imm_abc_typeid
imm_nuclt_typeid(struct imm_nuclt const *nuclt)
{
    return nuclt->super.vtable.typeid;
}

#endif
