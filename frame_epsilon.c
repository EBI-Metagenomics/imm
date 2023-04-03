#include "frame_epsilon.h"
#include "float_max.h"
#include <math.h>

struct imm_frame_epsilon imm_frame_epsilon(float epsilon)
{
  return (struct imm_frame_epsilon){
      epsilon == 0 ? -IMM_FLOAT_MAX : log(epsilon),
      epsilon == 1 ? -IMM_FLOAT_MAX : log(1 - epsilon)};
}
