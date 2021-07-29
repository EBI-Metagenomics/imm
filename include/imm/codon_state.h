#ifndef IMM_CODON_STATE_H
#define IMM_CODON_STATE_H

#include "imm/export.h"
#include "imm/state.h"

struct imm_state;
struct imm_codon_lprob;

struct imm_codon_state
{
    struct imm_state super;
    struct imm_codon_lprob const *codonp;
};

IMM_API void imm_codon_state_init(struct imm_codon_state *state, unsigned id,
                                  struct imm_codon_lprob const *codonp);

static inline struct imm_state *
imm_codon_state_super(struct imm_codon_state *codon_state)
{
    return &codon_state->super;
}

#endif
