#ifndef IMM_TRANS_H
#define IMM_TRANS_H

#include "htable.h"
#include "list.h"
#include "pair.h"
#include <stdint.h>

#define IMM_TRANS_NULL_IDX UINT16_MAX
#define IMM_TRANS_MAX_SIZE 15

struct imm_trans
{
  struct imm_pair pair;
  float lprob;
  struct imm_list outgoing;
  struct imm_list incoming;
  struct cco_hnode hnode;
};

void imm_trans_init(struct imm_trans *, unsigned src, unsigned dst,
                    float lprob);

#endif