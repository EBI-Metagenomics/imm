#ifndef IMM_CODON_LPROB_H
#define IMM_CODON_LPROB_H

#include "imm/bug.h"
#include "imm/codon.h"
#include "imm/lprob.h"
#include "imm/nuclt.h"

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
    imm_float lprobs[IMM_NUCLT_NSYMBOLS][IMM_NUCLT_NSYMBOLS]
                    [IMM_NUCLT_NSYMBOLS];
};

#define IMM_CODON_LPROB_INIT(nuclt)                                            \
    (struct imm_codon_lprob)                                                   \
    {                                                                          \
        nuclt,                                                                 \
        {                                                                      \
            [0 ... IMM_NUCLT_NSYMBOLS - 1] = {                                 \
                [0 ... IMM_NUCLT_NSYMBOLS -                                    \
                 1] = {[0 ... IMM_NUCLT_NSYMBOLS - 1] = IMM_LPROB_ZERO}        \
            }                                                                  \
        }                                                                      \
    }

static inline imm_float
imm_codon_lprob_get(struct imm_codon_lprob const *codonp,
                    struct imm_codon const *codon)
{
    IMM_BUG(codonp->nuclt != codon->nuclt);
    return codonp->lprobs[codon->a][codon->b][codon->c];
}

static inline void imm_codon_lprob_set(struct imm_codon_lprob *codonp,
                                       struct imm_codon const *codon,
                                       imm_float lprob)
{
    IMM_BUG(codonp->nuclt != codon->nuclt);
    codonp->lprobs[codon->a][codon->b][codon->c] = lprob;
}

static inline int imm_codon_lprob_normalize(struct imm_codon_lprob *codonp)
{
    return imm_lprob_normalize(IMM_NUCLT_NSYMBOLS * IMM_NUCLT_NSYMBOLS *
                                   IMM_NUCLT_NSYMBOLS,
                               &codonp->lprobs[0][0][0]);
}

#endif
