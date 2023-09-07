#include "table_state.h"

static float lprob(struct imm_state const *state, struct imm_seq const *seq)
{
  struct imm_table_state const *x = state->vtable.derived;
  return (*x->callback)(imm_seq_size(seq), imm_seq_str(seq));
}

void imm_table_state_init(struct imm_table_state *x, unsigned id,
                          struct imm_abc const *abc,
                          imm_table_state_callb *callb, struct imm_span span)
{
  x->callback = callb;
  struct imm_state_vtable vtable = {lprob, IMM_TABLE_STATE, x};
  imm_state_init(&x->super, id, abc, vtable, span);
}
