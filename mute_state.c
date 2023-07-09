#include "mute_state.h"
#include "lprob.h"
#include "span.h"
#include "state.h"

static float lprob(struct imm_state const *x, struct imm_seq const *seq)
{
  (void)x;
  if (imm_seq_size(seq) == 0) return log(1.0);
  return imm_lprob_zero();
}

void imm_mute_state_init(struct imm_mute_state *x, unsigned id,
                         struct imm_abc const *abc)
{
  struct imm_state_vtable vtable = {lprob, IMM_MUTE_STATE, x};
  imm_state_init(&x->super, id, abc, vtable, imm_span(0, 0));
}
