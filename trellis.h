#ifndef IMM_TRELLIS_H
#define IMM_TRELLIS_H

#include "export.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct imm_trans_table;

struct imm_trellis_node
{
  float score;
  uint16_t transition;
  uint8_t emission_length;
};

static_assert(sizeof(struct imm_trellis_node) == 8, "");

struct imm_trellis
{
  size_t capacity;
  unsigned sequence_size;
  unsigned num_states;
  struct imm_trellis_node *position;
  struct imm_trellis_node *pool;
};

IMM_API void imm_trellis_init(struct imm_trellis *);
IMM_API int imm_trellis_setup(struct imm_trellis *, unsigned sequence_size,
                              unsigned num_states);
IMM_API void imm_trellis_cleanup(struct imm_trellis *);

static inline void imm_trellis_rewind(struct imm_trellis *x)
{
  x->position = x->pool;
}

static inline void imm_trellis_push(struct imm_trellis *x, float score,
                                    uint16_t trans, uint8_t emislen)
{
  x->position->score = score;
  x->position->transition = trans;
  x->position->emission_length = emislen;
  x->position++;
}

static inline struct imm_trellis_node *imm_trellis_get(struct imm_trellis *x)
{
  return x->position;
}

IMM_API bool imm_trellis_step_back(struct imm_trellis *,
                                   struct imm_trans_table const *);

#endif
