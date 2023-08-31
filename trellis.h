#ifndef IMM_TRELLIS_H
#define IMM_TRELLIS_H

#include "api.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct imm_state_table;
struct imm_trans_table;

struct imm_trellis_node
{
  float score;
  uint16_t transition;
  uint8_t emission_size;
};

static_assert(sizeof(struct imm_trellis_node) == 8, "");

struct imm_trellis
{
  struct imm_state_table const *state_table;
  struct imm_trans_table const *trans_table;
  size_t capacity;
  unsigned sequence_size;
  unsigned num_states;
  struct imm_trellis_node *position;
  struct imm_trellis_node *pool;
};

IMM_API void imm_trellis_init(struct imm_trellis *,
                              struct imm_state_table const *,
                              struct imm_trans_table const *);
IMM_API int imm_trellis_setup(struct imm_trellis *, unsigned sequence_size,
                              unsigned num_states);
IMM_API void imm_trellis_cleanup(struct imm_trellis *);

static inline void imm_trellis_rewind(struct imm_trellis *x)
{
  x->position = x->pool;
}

static inline void imm_trellis_seek(struct imm_trellis *x, unsigned seqpos,
                                    unsigned state)
{
  x->position = &x->pool[seqpos * x->num_states + state];
}

static inline void imm_trellis_push(struct imm_trellis *x, float score,
                                    uint16_t trans, uint8_t emissize)
{
  x->position->score = score;
  x->position->transition = trans;
  x->position->emission_size = emissize;
  x->position++;
}

static inline struct imm_trellis_node *
imm_trellis_get(struct imm_trellis const *x)
{
  return x->position;
}

static inline unsigned imm_trellis_state_idx(struct imm_trellis const *x)
{
  return (x->position - x->pool) % x->num_states;
}

IMM_API unsigned imm_trellis_state_id(struct imm_trellis const *);

static inline unsigned imm_trellis_sequence_idx(struct imm_trellis const *x)
{
  return (x->position - x->pool) / x->num_states;
}

IMM_API bool imm_trellis_step_back(struct imm_trellis *);

IMM_API void imm_trellis_dump(struct imm_trellis const *x, FILE *restrict);

#endif
