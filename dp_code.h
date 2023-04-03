#ifndef IMM_DP_CODE_H
#define IMM_DP_CODE_H

#include <stdint.h>

struct imm_abc;

struct imm_dp_code
{
  struct
  {
    unsigned min;
    unsigned max;
  } seqlen;
  uint16_t *offset;
  uint16_t *stride;
  unsigned size;
  struct imm_abc const *abc;
};

#endif
