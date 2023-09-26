#ifndef IMM_STEP_H
#define IMM_STEP_H

#include "api.h"
#include "state.h"
#include "static_assert.h"
#include <assert.h>
#include <stdalign.h>
#include <stdint.h>

struct imm_step
{
  uint16_t state_id;
  uint8_t seqlen;
  float score;
};

imm_static_assert(sizeof(struct imm_step) == 8, "struct pair must be packed");
imm_static_assert(alignof(struct imm_step) == 4, "struct pair must align to 4");

IMM_API struct imm_step imm_step(unsigned state_id, unsigned seqlen,
                                 float score);

#endif
