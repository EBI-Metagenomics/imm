#include "step.h"

struct imm_step imm_step(int state_id, int seqsize, float score)
{
  return (struct imm_step){(int16_t)state_id, (int8_t)seqsize, score};
}
