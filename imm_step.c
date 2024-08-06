#include "imm_step.h"

struct imm_step imm_step(int state_id, int seqsize, float score)
{
  return (struct imm_step){(uint16_t)state_id, (int8_t)seqsize, score};
}
