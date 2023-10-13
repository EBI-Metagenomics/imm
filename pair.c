#include "pair.h"
#include "state.h"

struct imm_pair imm_pair(int src_id, int dst_id)
{
  return (struct imm_pair){.id.src = (int16_t)src_id,
                           .id.dst = (int16_t)dst_id,
                           .idx.src = IMM_STATE_NULL_IDX,
                           .idx.dst = IMM_STATE_NULL_IDX};
}
