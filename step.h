#ifndef IMM_STEP_H
#define IMM_STEP_H

#include "export.h"
#include "state.h"
#include <assert.h>
#include <stdalign.h>
#include <stdint.h>

struct imm_step
{
  uint16_t state_id;
  uint8_t seqlen;
};

static_assert(sizeof(struct imm_step) == 4, "struct pair must be packed");
static_assert(alignof(struct imm_step) == 2, "struct pair must align to 2");

IMM_API struct imm_step imm_step(unsigned state_id, unsigned seqlen);

#endif
