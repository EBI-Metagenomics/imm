#ifndef IMM_NODE_H
#define IMM_NODE_H

#include "compiler.h"
#include "lprob.h"
#include "state.h"
#include "static_assert.h"
#include <stdint.h>
#include <stdio.h>

struct imm_node
{
  float score;
  uint16_t state_source;
  uint8_t emission_size;
};

struct imm_state_table;

// TODO: I would like this to be 4 bytes instead
imm_static_assert(sizeof(struct imm_node) == 8, "");

IMM_API void imm_node_dump(struct imm_node const *, uint16_t *ids,
                           imm_state_name *, FILE *restrict);

IMM_INLINE void imm_node_invalidate(struct imm_node *x)
{
  x->score = IMM_LPROB_NAN;
  x->state_source = IMM_STATE_NULL_IDX;
  x->emission_size = IMM_STATE_NULL_SEQLEN;
}

#endif
