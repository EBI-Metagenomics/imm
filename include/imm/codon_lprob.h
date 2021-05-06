#ifndef IMM_CODON_LPROB_H
#define IMM_CODON_LPROB_H

#include "imm/arr3d.h"
#include "imm/codon.h"
#include "imm/compiler.h"
#include "imm/error.h"
#include "imm/export.h"
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
    struct imm_arr3d lprobs;
};

IMM_API void imm_codon_lprob_init(struct imm_codon_lprob *codonp,
                                  struct imm_nuclt const *nuclt);

IMM_API void imm_codon_lprob_deinit(struct imm_codon_lprob *codonp);

static inline bool __imm_codon_lprob_check(struct imm_codon_lprob const *codonp,
                                           struct imm_codon const *codon)
{
    if (imm_unlikely(codonp->nuclt != codon->nuclt))
    {
        imm_log_error("alphabets must be the same");
        return false;
    }

    return true;
}

static inline imm_float
imm_codon_lprob_get(struct imm_codon_lprob const *codonp,
                    struct imm_codon const *codon)
{
    if (imm_unlikely(!__imm_codon_lprob_check(codonp, codon)))
        return imm_lprob_nan();

    return imm_arr3d_get(&codonp->lprobs, codon->idx);
}

static inline int imm_codon_lprob_set(struct imm_codon_lprob *codonp,
                                      struct imm_codon const *codon,
                                      imm_float lprob)
{
    if (imm_unlikely(!__imm_codon_lprob_check(codonp, codon)))
        return IMM_ILLEGALARG;

    imm_arr3d_set(&codonp->lprobs, codon->idx, lprob);
    return IMM_SUCCESS;
}

static inline int imm_codon_lprob_normalize(struct imm_codon_lprob *codonp)
{
    return imm_arr3d_normalize(&codonp->lprobs);
}

#endif
