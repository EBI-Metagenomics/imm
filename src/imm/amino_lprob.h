#ifndef IMM_AMINO_LPROB_H
#define IMM_AMINO_LPROB_H

#include "imm/amino.h"
#include "imm/generics.h"
#include "imm/lprob.h"
#include <assert.h>

struct imm_amino_lprob
{
    struct imm_amino const *amino;
    imm_float lprobs[IMM_AMINO_SIZE];
};

static inline struct imm_amino_lprob
imm_amino_lprob(struct imm_amino const *amino,
                imm_float const lprobs[IMM_AMINO_SIZE])
{
    struct imm_amino_lprob lprob;
    lprob.amino = amino;
    for (unsigned i = 0; i < IMM_AMINO_SIZE; ++i)
        lprob.lprobs[i] = lprobs[i];
    return lprob;
}

static inline imm_float
__imm_amino_lprob_get(struct imm_amino_lprob const *lprob, unsigned idx)
{
    return lprob->lprobs[idx];
}

static inline imm_float imm_amino_lprob_get(struct imm_amino_lprob const *lprob,
                                            char symbol)
{
    struct imm_abc const *abc = imm_super(lprob->amino);
    assert(imm_abc_has_symbol(abc, symbol));
    return __imm_amino_lprob_get(lprob, imm_abc_symbol_idx(abc, symbol));
}

#endif
