#ifndef IMM_ABC_LPROB_H
#define IMM_ABC_LPROB_H

#include "imm/abc.h"
#include "imm/compiler.h"
#include "imm/error.h"
#include "imm/export.h"
#include "imm/log.h"
#include "imm/lprob.h"

struct imm_abc_lprob
{
    struct imm_abc const *abc;
    imm_float const *lprobs;
};

static inline struct imm_abc const *
imm_abc_lprob_abc(struct imm_abc_lprob const *abc_lprob)
{
    return abc_lprob->abc;
}

static inline void imm_abc_lprob_init(struct imm_abc_lprob *lprob,
                                      struct imm_abc const *abc,
                                      imm_float const *lprobs)
{
    lprob->abc = abc;
    lprob->lprobs = lprobs;
}

static inline imm_float imm_abc_lprob_get(struct imm_abc_lprob const *abc_lprob,
                                          char symbol)
{
    unsigned idx = imm_abc_symbol_idx(abc_lprob->abc, symbol);
    if (imm_unlikely(idx == IMM_SYM_NULL_IDX))
    {
        imm_log_error("symbol not found");
        return imm_lprob_nan();
    }
    return abc_lprob->lprobs[idx];
}

#endif
