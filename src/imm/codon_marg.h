#ifndef IMM_CODON_MARG_H
#define IMM_CODON_MARG_H

#include "imm/codon.h"
#include "imm/export.h"
#include "imm/float.h"

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
     * Pre-computed marginalization forms of
     * p(𝑋₁=𝚡₁,𝑋₂=𝚡₂,𝑋₃=𝚡₃).
     */
    imm_float lprobs[IMM_NUCLT_SIZE + 1][IMM_NUCLT_SIZE + 1]
                    [IMM_NUCLT_SIZE + 1];
};

struct lip_ctx_file;

IMM_API struct imm_codon_marg imm_codon_marg(struct imm_codon_lprob *codonp);

/**
 * Calculate any of the marginalization forms of
 * p(𝑋₁=𝚡₁,𝑋₂=𝚡₂,𝑋₃=𝚡₃).
 *
 * The alphabet's any-symbol can be passed to @codon to perform marginalization
 * over the corresponding random variable. Let `"ACGT"` be a set of nucleotides
 * and let `'X`' be the any-symbol of the given alphabet. The code
 *
 *     imm_codon_set(&codon, 0, 5, 2);
 *     imm_codon_marg_lprob(&codonm, &codon);
 *
 * will evaluate the probability p(𝑋₁=𝙰,𝑋₃=𝙶).
 */
static inline imm_float
imm_codon_marg_lprob(struct imm_codon_marg const *codonm,
                     struct imm_codon codon)
{
    return codonm->lprobs[codon.idx[0]][codon.idx[1]][codon.idx[2]];
}

IMM_API enum imm_rc imm_codon_marg_pack(struct imm_codon_marg const *codonm,
                                        struct lip_ctx_file *ctx);

IMM_API enum imm_rc imm_codon_marg_unpack(struct imm_codon_marg *codonm,
                                          struct lip_ctx_file *ctx);

#endif
