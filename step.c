#include "step.h"

struct imm_step imm_step(unsigned state_id, unsigned seqlen, float score)
{
  return (struct imm_step){(uint16_t)state_id, (uint8_t)seqlen, score};
}
