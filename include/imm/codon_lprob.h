#ifndef IMM_CODON_LPROB_H
#define IMM_CODON_LPROB_H

#include "imm/arr3d.h"
#include "imm/codon.h"
#include "imm/compiler.h"
#include "imm/error.h"
#include "imm/export.h"
#include "imm/lprob.h"
#include "imm/nuclt.h"

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
    struct imm_abc const *abc = imm_nuclt_super(codonp->nuclt);

    if (imm_unlikely(codonp->nuclt != codon->nuclt))
    {
        imm_log_error("alphabets must be the same");
        return false;
    }

    if (imm_unlikely(!(imm_abc_has_symbol(abc, codon->triplet.a) &&
                       imm_abc_has_symbol(abc, codon->triplet.b) &&
                       imm_abc_has_symbol(abc, codon->triplet.c))))
    {
        imm_log_error("invalid codon symbol");
        return false;
    }
    return true;
}

static inline imm_float
imm_codon_lprob_get(struct imm_codon_lprob const *codonp,
                    struct imm_codon const *codon)
{
    struct imm_abc const *abc = imm_nuclt_super(codonp->nuclt);

    if (imm_unlikely(!__imm_codon_lprob_check(codonp, codon)))
        return imm_lprob_nan();

    return imm_arr3d_get(&codonp->lprobs,
                         imm_abc_symbol_idx(abc, codon->triplet.a),
                         imm_abc_symbol_idx(abc, codon->triplet.b),
                         imm_abc_symbol_idx(abc, codon->triplet.c));
}

static inline int imm_codon_lprob_set(struct imm_codon_lprob *codonp,
                                      struct imm_codon const *codon,
                                      imm_float lprob)
{
    struct imm_abc const *abc = imm_nuclt_super(codonp->nuclt);

    if (imm_unlikely(!__imm_codon_lprob_check(codonp, codon)))
        return IMM_ILLEGALARG;

    imm_arr3d_set(&codonp->lprobs, imm_abc_symbol_idx(abc, codon->triplet.a),
                  imm_abc_symbol_idx(abc, codon->triplet.b),
                  imm_abc_symbol_idx(abc, codon->triplet.c), lprob);
    return IMM_SUCCESS;
}

static inline int imm_codon_lprob_normalize(struct imm_codon_lprob *codonp)
{
    return imm_arr3d_normalize(&codonp->lprobs);
}

#endif
