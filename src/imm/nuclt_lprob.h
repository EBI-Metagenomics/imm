#ifndef IMM_NUCLT_LPROB_H
#define IMM_NUCLT_LPROB_H

#include "imm/log.h"
#include "imm/lprob.h"
#include "imm/nuclt.h"
#include <assert.h>

struct imm_nuclt_lprob
{
    struct imm_nuclt const *nuclt;
    imm_float lprobs[IMM_NUCLT_SIZE];
};

struct cw_pack_context;
struct cw_unpack_context;

static inline struct imm_nuclt_lprob
imm_nuclt_lprob(struct imm_nuclt const *nuclt,
                imm_float const lprobs[IMM_NUCLT_SIZE])
{
    struct imm_nuclt_lprob lprob;
    lprob.nuclt = nuclt;
    for (unsigned i = 0; i < IMM_NUCLT_SIZE; ++i)
        lprob.lprobs[i] = lprobs[i];
    return lprob;
}

static inline imm_float
__imm_nuclt_lprob_get(struct imm_nuclt_lprob const *lprob, unsigned idx)
{
    return lprob->lprobs[idx];
}

static inline imm_float imm_nuclt_lprob_get(struct imm_nuclt_lprob const *lprob,
                                            char symbol)
{
    struct imm_abc const *abc = imm_nuclt_super(lprob->nuclt);
    assert(imm_abc_has_symbol(abc, symbol));
    return __imm_nuclt_lprob_get(lprob, imm_abc_symbol_idx(abc, symbol));
}

IMM_API enum imm_rc imm_nuclt_lprob_pack(struct imm_nuclt_lprob const *nucltp,
                                         struct cw_pack_context *ctx);

IMM_API enum imm_rc imm_nuclt_lprob_unpack(struct imm_nuclt_lprob *nucltp,
                                           struct cw_unpack_context *ctx);

#endif
