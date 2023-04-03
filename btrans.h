#ifndef IMM_BTRANS_H
#define IMM_BTRANS_H

#include "lprob.h"
#include "state.h"

struct imm_btrans
{
  float score;
  unsigned prev_state;
  unsigned trans;
  unsigned len;
};

struct imm_btrans imm_btrans_init(void);
void imm_btrans_set(struct imm_btrans *, float score, unsigned prev_state,
                    unsigned trans, unsigned len);

#endif
