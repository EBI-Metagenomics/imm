#ifndef IMM_TRELLIS_H
#define IMM_TRELLIS_H

#include "api.h"
#include "lprob.h"
#include "node.h"
#include "state.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct imm_state_table;
struct imm_trans_table;

struct imm_trellis
{
  struct imm_state_table const *state_table;
  struct imm_trans_table const *trans_table;
  size_t capacity;
  unsigned sequence_size;
  unsigned num_states;
  struct imm_node *head;
  struct imm_node *pool;
};

IMM_API void imm_trellis_init(struct imm_trellis *,
                              struct imm_state_table const *,
                              struct imm_trans_table const *);
IMM_API int imm_trellis_setup(struct imm_trellis *, unsigned seqsize,
                              unsigned nstates);
IMM_API void imm_trellis_cleanup(struct imm_trellis *);

IMM_API unsigned imm_trellis_state_id(struct imm_trellis const *);
IMM_API bool imm_trellis_back(struct imm_trellis *);
IMM_API void imm_trellis_dump(struct imm_trellis const *, imm_state_name *,
                              FILE *restrict);

IMM_INLINE void imm_trellis_rewind(struct imm_trellis *x) { x->head = x->pool; }

IMM_CONST size_t imm_trellis_size(struct imm_trellis const *x)
{
  return x->sequence_size * x->num_states;
}

IMM_CONST bool imm_trellis_has_back(struct imm_trellis const *x)
{
  return !imm_lprob_is_nan(x->head->score);
}

IMM_PURE struct imm_node *imm_trellis_head(struct imm_trellis const *x)
{
  return x->head;
}

IMM_PURE struct imm_node const *imm_trellis_at(struct imm_trellis const *x,
                                               unsigned seqpos, unsigned state)
{
  return x->pool + seqpos * x->num_states + state;
}

IMM_INLINE void imm_trellis_seek(struct imm_trellis *x, unsigned seqpos,
                                 unsigned state)
{
  x->head = (struct imm_node *)imm_trellis_at(x, seqpos, state);
  assert(x->head >= x->pool && x->head < x->pool + imm_trellis_size(x));
}

IMM_INLINE void imm_trellis_push(struct imm_trellis *x, float score,
                               uint16_t state_source, uint8_t emissize)
{
  x->head->score = score;
  x->head->state_source = state_source;
  x->head->emission_size = emissize;
  x->head++;
}

IMM_PURE unsigned imm_trellis_state_idx_at(struct imm_trellis const *x,
                                           struct imm_node const *head)
{
  return (head - x->pool) % x->num_states;
}

IMM_PURE unsigned imm_trellis_state_idx(struct imm_trellis const *x)
{
  return imm_trellis_state_idx_at(x, x->head);
}

IMM_PURE unsigned imm_trellis_sequence_idx(struct imm_trellis const *x)
{
  return (x->head - x->pool) / x->num_states;
}

#endif
