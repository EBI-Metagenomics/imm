#ifndef IMM_CTRANS_H
#define IMM_CTRANS_H

#include <assert.h>
#include <stdint.h>

struct imm_ctrans
{
  float score;  /**< Transition score.  */
  uint16_t src; /**< Source state.      */
  uint16_t dst; /**< Destination state. */
};

static_assert(sizeof(struct imm_ctrans) == 8, "");

#endif
