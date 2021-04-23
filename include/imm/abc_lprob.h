#ifndef IMM_ABC_LPROB_H
#define IMM_ABC_LPROB_H

#include "imm/abc.h"
#include "imm/export.h"
#include "imm/float.h"
#include "imm/log.h"
#include "imm/lprob.h"
#include <stdlib.h>

struct imm_abc_lprob
{
    struct imm_abc const *abc;
    imm_float lprobs[];
};

static inline struct imm_abc const *
imm_abc_lprob_abc(struct imm_abc_lprob const *abc_lprob)
{
    return abc_lprob->abc;
}

IMM_API struct imm_abc_lprob const *imm_abc_lprob_new(struct imm_abc const *abc,
                                                      imm_float const *lprobs);

static inline void imm_abc_lprob_del(struct imm_abc_lprob const *abc_lprob)
{
    free((void *)abc_lprob);
}

static inline imm_float imm_abc_lprob_get(struct imm_abc_lprob const *abc_lprob,
                                          char symbol)
{
    uint8_t idx = imm_abc_symbol_idx(abc_lprob->abc, symbol);
    if (idx == IMM_ABC_INVALID_IDX)
    {
        imm_log_error("symbol not found");
        return imm_lprob_invalid();
    }
    return abc_lprob->lprobs[idx];
}

#endif
