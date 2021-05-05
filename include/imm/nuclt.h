#ifndef IMM_NUCLT_H
#define IMM_NUCLT_H

#include "imm/abc.h"
#include "imm/export.h"

#define IMM_NUCLT_ANY_SYMBOL 'X'
#define IMM_NUCLT_SYMBOLS "ACGT"
#define IMM_NUCLT_NSYMBOLS 4

struct imm_nuclt
{
    struct imm_abc super;
};

IMM_API int imm_nuclt_init(struct imm_nuclt *nuclt,
                           char const symbols[IMM_NUCLT_NSYMBOLS],
                           char any_symbol);

static inline struct imm_abc const *
imm_nuclt_super(struct imm_nuclt const *nuclt)
{
    return &nuclt->super;
}

#endif
