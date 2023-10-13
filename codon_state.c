#include "codon_state.h"
#include "codon_lprob.h"
#include "likely.h"
#include "lprob.h"

static float lprob(struct imm_state const *state, struct imm_seq const *seq)
{
  struct imm_codon_state const *codon_state = state->vtable.derived;
  if (imm_unlikely(imm_seq_size(seq) != 3)) return imm_lprob_nan();

  struct imm_codon_lprob const *codonp = codon_state->codonp;
  struct imm_codon codon = {
      .nuclt = codonp->nuclt,
      .a = imm_abc_symbol_idx(state->abc, imm_seq_data(seq)[0]),
      .b = imm_abc_symbol_idx(state->abc, imm_seq_data(seq)[1]),
      .c = imm_abc_symbol_idx(state->abc, imm_seq_data(seq)[2])};

  return imm_codon_lprob_get(codonp, codon);
}

void imm_codon_state_init(struct imm_codon_state *state, int id,
                          struct imm_codon_lprob const *codonp)
{
  state->codonp = codonp;
  struct imm_state_vtable vtable = {lprob, IMM_CODON_STATE, state};
  struct imm_abc const *abc = &codonp->nuclt->super;
  imm_state_init(&state->super, id, abc, vtable, imm_span(3, 3));
}
