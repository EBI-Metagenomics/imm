#include "imm/frame_epsilon.h"

struct imm_frame_epsilon imm_frame_epsilon(imm_float epsilon)
{
    return (struct imm_frame_epsilon){
        epsilon == 0 ? -IMM_FLOAT_MAX : imm_log(epsilon),
        epsilon == 1 ? -IMM_FLOAT_MAX : imm_log(1 - epsilon)};
}
