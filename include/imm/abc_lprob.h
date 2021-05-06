#ifndef IMM_ABC_LPROB_H
#define IMM_ABC_LPROB_H

#include "imm/abc.h"
#include "imm/bug.h"
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

static inline void imm_abc_lprob_init(struct imm_abc_lprob *lprob,
                                      struct imm_abc const *abc,
                                      imm_float const *lprobs)
{
    lprob->abc = abc;
    lprob->lprobs = lprobs;
}

static inline imm_float imm_abc_lprob_get(struct imm_abc_lprob const *lprob,
                                          char symbol)
{
    IMM_BUG(!imm_abc_has_symbol(lprob->abc, symbol));
    return lprob->lprobs[imm_abc_symbol_idx(lprob->abc, symbol)];
}

#endif
