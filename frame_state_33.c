#include "frame_state_33.h"
#include "codon_marg.h"
#include "frame_state.h"
#include "lprob.h"
#include "nuclt_lprob.h"

#define codon(A, B, C)                                                         \
  (struct imm_codon)                                                           \
  {                                                                            \
    .a = (A), .b = (B), .c = (C)                                               \
  }

#define lf(A, B, C)                                                            \
  imm_codon_marg_lprob(state->codonm, codon(nucl[A], nucl[B], nucl[C]))

// p(S=(s0,s1,s2))
float imm_joint_n3_33(struct imm_frame_state const *state,
                      struct imm_seq const *seq)
{
  struct imm_abc const *abc = &state->codonm->nuclt->super;
  unsigned const nucl[4] = {imm_abc_symbol_idx(abc, imm_seq_str(seq)[0]),
                            imm_abc_symbol_idx(abc, imm_seq_str(seq)[1]),
                            imm_abc_symbol_idx(abc, imm_seq_str(seq)[2]),
                            imm_abc_any_symbol_idx(abc)};
  return lf(0, 1, 2);
}
