#include "step.h"

struct imm_step imm_step(unsigned state_id, unsigned seqlen)
{
  return (struct imm_step){(uint16_t)state_id, (uint8_t)seqlen};
}
