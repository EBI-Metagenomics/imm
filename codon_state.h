#ifndef IMM_CODON_STATE_H
#define IMM_CODON_STATE_H

#include "api.h"
#include "state.h"

struct imm_codon_lprob;

struct imm_codon_state
{
  struct imm_state super;
  struct imm_codon_lprob const *codonp;
};

IMM_API void imm_codon_state_init(struct imm_codon_state *, unsigned id,
                                  struct imm_codon_lprob const *);

#endif
