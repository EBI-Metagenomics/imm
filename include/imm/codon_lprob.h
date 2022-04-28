#ifndef IMM_CODON_LPROB_H
#define IMM_CODON_LPROB_H

#include "imm/codon.h"
#include "imm/log.h"
#include "imm/lprob.h"
#include "imm/nuclt.h"
#include "imm/rc.h"
#include <assert.h>

/** @file codon_lprob.h
 * Codon probability module.
 *
 * An object of type @ref imm_codon_lprob is used to define the probabilities
 * p(𝑋₁=𝚡₁,𝑋₂=𝚡₂,𝑋₃=𝚡₃) of emitting codon (𝚡₁,𝚡₂,𝚡₃). Its sole purpose is
 * to be used by the constructor of the type @ref imm_codon_marg.
 */

struct imm_codon_lprob
{
    struct imm_nuclt const *nuclt;
    /**
     * Pre-computed probability p(𝑋₁=𝚡₁,𝑋₂=𝚡₂,𝑋₃=𝚡₃).
     */
    imm_float lprobs[IMM_NUCLT_SIZE * IMM_NUCLT_SIZE * IMM_NUCLT_SIZE];
};

#define IMM_CODON_STRIDE(a, b, c)                                              \
    (a * IMM_NUCLT_SIZE * IMM_NUCLT_SIZE + b * IMM_NUCLT_SIZE + c)

static inline struct imm_codon_lprob
imm_codon_lprob(struct imm_nuclt const *nuclt)
{
    struct imm_codon_lprob lprob;
    lprob.nuclt = nuclt;

    unsigned i = 0;
    for (; i < IMM_NUCLT_SIZE * IMM_NUCLT_SIZE * IMM_NUCLT_SIZE; ++i)
        lprob.lprobs[i] = IMM_LPROB_ZERO;

    return lprob;
}

static inline imm_float
imm_codon_lprob_get(struct imm_codon_lprob const *codonp,
                    struct imm_codon codon)
{
    assert(codonp->nuclt == codon.nuclt);
    return codonp->lprobs[IMM_CODON_STRIDE(codon.a, codon.b, codon.c)];
}

static inline void imm_codon_lprob_set(struct imm_codon_lprob *codonp,
                                       struct imm_codon codon, imm_float lprob)
{
    assert(codonp->nuclt == codon.nuclt);
    codonp->lprobs[IMM_CODON_STRIDE(codon.a, codon.b, codon.c)] = lprob;
}

static inline void imm_codon_lprob_normalize(struct imm_codon_lprob *codonp)
{
    imm_lprob_normalize(IMM_NUCLT_SIZE * IMM_NUCLT_SIZE * IMM_NUCLT_SIZE,
                        codonp->lprobs);
}

#endif
