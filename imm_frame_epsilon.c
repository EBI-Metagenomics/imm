#include "imm_frame_epsilon.h"
#include <float.h>
#include <math.h>

struct imm_frame_epsilon imm_frame_epsilon(float epsilon)
{
  return (struct imm_frame_epsilon){epsilon == 0 ? -FLT_MAX : logf(epsilon),
                                    epsilon == 1 ? -FLT_MAX
                                                 : logf(1 - epsilon)};
}
