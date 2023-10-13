#ifndef IMM_CODON_STATE_H
#define IMM_CODON_STATE_H

#include "compiler.h"
#include "state.h"

struct imm_codon_lprob;

struct imm_codon_state
{
  struct imm_state super;
  struct imm_codon_lprob const *codonp;
};

IMM_API void imm_codon_state_init(struct imm_codon_state *, int id,
                                  struct imm_codon_lprob const *);

#endif
