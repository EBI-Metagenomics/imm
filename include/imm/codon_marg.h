#ifndef IMM_CODON_MARG_H
#define IMM_CODON_MARG_H

#include "imm/arr3d.h"
#include "imm/codon.h"
#include "imm/export.h"

/** @file codon_marg.h
 * Codon marginalization module.
 *
 * A codon marginalization is represented by an (immutable) object of type @ref
 * imm_codon_marg and is used to compute the marginalization forms of
 * p(𝑋₁=𝚡₁,𝑋₂=𝚡₂,𝑋₃=𝚡₃), the probability of emitting codon (𝚡₁,𝚡₂,𝚡₃).
 */

struct imm_codon_lprob;

struct imm_codon_marg
{
    struct imm_nuclt const *nuclt;
    /**
     * Maps alphabet symbols to the indices 0, 1, 2, and 3 and the any-symbol
     * to 4.
     */
    imm_sym_idx_t symbol_idx[IMM_SYM_SIZE + 1];
    /**
     * Pre-computed marginalization forms of
     * p(𝑋₁=𝚡₁,𝑋₂=𝚡₂,𝑋₃=𝚡₃).
     */
    struct imm_arr3d lprobs;
};

IMM_API void imm_codon_marg_init(struct imm_codon_marg *codonm,
                                 struct imm_codon_lprob *codonp,
                                 struct imm_nuclt const *nuclt);

IMM_API void imm_codon_marg_deinit(struct imm_codon_marg *codonm);

static inline imm_float
imm_codon_marg_lprob(struct imm_codon_marg const *codonm,
                     struct imm_codon const *codon);

/**
 * Calculate any of the marginalization forms of
 * p(𝑋₁=𝚡₁,𝑋₂=𝚡₂,𝑋₃=𝚡₃).
 *
 * The alphabet's any-symbol can be passed to @codon to perform marginalization
 * over the corresponding random variable. Let `"ACGT"` be a set of nucleotides
 * and let `'X`' be the any-symbol of the given alphabet. The code
 *
 *     imm_codon_set(&codon, IMM_TRIPLET_INIT('A', 'X', 'G'));
 *     imm_codon_marg_lprob(&codonm, &codon);
 *
 * will evaluate the probability p(𝑋₁=𝙰,𝑋₃=𝙶).
 */
static inline imm_float
imm_codon_marg_lprob(struct imm_codon_marg const *codonm,
                     struct imm_codon const *codon)
{
    return imm_arr3d_get(&codonm->lprobs, codon->idx.data);
}

#endif
