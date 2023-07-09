#include "frame_epsilon.h"
#include <float.h>
#include <math.h>

struct imm_frame_epsilon imm_frame_epsilon(float epsilon)
{
  return (struct imm_frame_epsilon){epsilon == 0 ? -FLT_MAX : log(epsilon),
                                    epsilon == 1 ? -FLT_MAX : log(1 - epsilon)};
}
