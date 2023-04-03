#include "pair.h"
#include "state.h"

struct imm_pair imm_pair(unsigned src_id, unsigned dst_id)
{
  return (struct imm_pair){.id.src = (uint16_t)src_id,
                           .id.dst = (uint16_t)dst_id,
                           .idx.src = IMM_STATE_NULL_IDX,
                           .idx.dst = IMM_STATE_NULL_IDX};
}
