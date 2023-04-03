#ifndef IMM_CODON_LPROB_H
#define IMM_CODON_LPROB_H

#include "codon.h"
#include "export.h"
#include <assert.h>

struct imm_nuclt;

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
  float lprobs[IMM_NUCLT_SIZE * IMM_NUCLT_SIZE * IMM_NUCLT_SIZE];
};

#define IMM_CODON_STRIDE(a, b, c)                                              \
  (a * IMM_NUCLT_SIZE * IMM_NUCLT_SIZE + b * IMM_NUCLT_SIZE + c)

IMM_API struct imm_codon_lprob imm_codon_lprob(struct imm_nuclt const *);
IMM_API float imm_codon_lprob_get(struct imm_codon_lprob const *,
                                  struct imm_codon);
IMM_API void imm_codon_lprob_set(struct imm_codon_lprob *, struct imm_codon,
                                 float lprob);
IMM_API void imm_codon_lprob_normalize(struct imm_codon_lprob *);

#endif
