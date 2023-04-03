#include "state.h"
#include "likely.h"
#include "list.h"
#include "lprob.h"

struct imm_abc const *imm_state_abc(struct imm_state const *x)
{
  return x->abc;
}

unsigned imm_state_id(struct imm_state const *x) { return x->id; }

unsigned imm_state_idx(struct imm_state const *x) { return x->idx; }

float imm_state_lprob(struct imm_state const *x, struct imm_seq const *seq)
{
  if (imm_unlikely(x->abc != imm_seq_abc(seq))) return imm_lprob_nan();
  return x->vtable.lprob(x, seq);
}

struct imm_span imm_state_span(struct imm_state const *x) { return x->span; }

enum imm_state_typeid imm_state_typeid(struct imm_state const *x)
{
  return x->vtable.typeid;
}

void imm_state_detach(struct imm_state *x)
{
  imm_list_init(&x->trans.outgoing);
  imm_list_init(&x->trans.incoming);
  cco_hash_del(&x->hnode);
}

void imm_state_init(struct imm_state *x, unsigned id, struct imm_abc const *abc,
                    struct imm_state_vtable vtable, struct imm_span span)
{
  assert(id != IMM_STATE_NULL_ID);
  x->id = id;
  x->idx = IMM_STATE_NULL_IDX;
  x->abc = abc;
  x->span = span;
  x->vtable = vtable;
  imm_list_init(&x->trans.outgoing);
  imm_list_init(&x->trans.incoming);
  cco_hnode_init(&x->hnode);
  x->mark = 0;
}
