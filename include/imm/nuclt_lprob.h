#ifndef IMM_NUCLT_LPROB_H
#define IMM_NUCLT_LPROB_H

#include "imm/bug.h"
#include "imm/generics.h"
#include "imm/lprob.h"
#include "imm/nuclt.h"

struct imm_nuclt_lprob
{
    struct imm_nuclt const *nuclt;
    imm_float lprobs[IMM_NUCLT_NSYMBOLS];
};

static inline void
imm_nuclt_lprob_init(struct imm_nuclt_lprob *lprob,
                     struct imm_nuclt const *nuclt,
                     imm_float const lprobs[IMM_NUCLT_NSYMBOLS])
{
    lprob->nuclt = nuclt;
    for (unsigned i = 0; i < IMM_NUCLT_NSYMBOLS; ++i)
        lprob->lprobs[i] = lprobs[i];
}

static inline imm_float
__imm_nuclt_lprob_get(struct imm_nuclt_lprob const *lprob, unsigned idx)
{
    return lprob->lprobs[idx];
}

static inline imm_float imm_nuclt_lprob_get(struct imm_nuclt_lprob const *lprob,
                                            char symbol)
{
    struct imm_abc const *abc = imm_super(lprob->nuclt);
    IMM_BUG(!imm_nuclt_has_symbol(abc, symbol));
    return __imm_nuclt_lprob_get(lprob, imm_abc_symbol_idx(abc, symbol));
}

#endif
