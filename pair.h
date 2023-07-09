#ifndef IMM_PAIR_H
#define IMM_PAIR_H

#include "api.h"
#include <assert.h>
#include <stdint.h>

struct imm_pair
{
  struct
  {
    union
    {
      struct
      {
        uint16_t src;
        uint16_t dst;
      };
      uint32_t key;
    };
  } id;

  struct
  {
    struct
    {
      uint16_t src;
      uint16_t dst;
    };
  } idx;
};

static_assert(sizeof(struct imm_pair) == 8, "struct pair must be packed");

IMM_API struct imm_pair imm_pair(unsigned src_id, unsigned dst_id);

#endif
