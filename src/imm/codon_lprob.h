#ifndef IMM_CODON_LPROB_H
#define IMM_CODON_LPROB_H

#include "imm/codon.h"
#include "imm/log.h"
#include "imm/lprob.h"
#include "imm/nuclt.h"
#include "imm/rc.h"

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
    imm_float lprobs[IMM_NUCLT_SIZE][IMM_NUCLT_SIZE][IMM_NUCLT_SIZE];
};

static inline struct imm_codon_lprob
imm_codon_lprob(struct imm_nuclt const *nuclt)
{
    struct imm_codon_lprob lprob;
    lprob.nuclt = nuclt;

    for (unsigned a = 0; a < IMM_NUCLT_SIZE; ++a)
        for (unsigned b = 0; b < IMM_NUCLT_SIZE; ++b)
            for (unsigned c = 0; c < IMM_NUCLT_SIZE; ++c)
                lprob.lprobs[a][b][c] = IMM_LPROB_ZERO;

    return lprob;
}

static inline imm_float
imm_codon_lprob_get(struct imm_codon_lprob const *codonp,
                    struct imm_codon codon)
{
    __IMM_BUG(codonp->nuclt != codon.nuclt);
    return codonp->lprobs[codon.a][codon.b][codon.c];
}

static inline void imm_codon_lprob_set(struct imm_codon_lprob *codonp,
                                       struct imm_codon codon, imm_float lprob)
{
    __IMM_BUG(codonp->nuclt != codon.nuclt);
    codonp->lprobs[codon.a][codon.b][codon.c] = lprob;
}

static inline enum imm_rc
imm_codon_lprob_normalize(struct imm_codon_lprob *codonp)
{
    return imm_lprob_normalize(IMM_NUCLT_SIZE * IMM_NUCLT_SIZE * IMM_NUCLT_SIZE,
                               &codonp->lprobs[0][0][0]);
}

#endif
