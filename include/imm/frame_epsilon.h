#ifndef IMM_FRAME_EPSILON_H
#define IMM_FRAME_EPSILON_H

#include "imm/float.h"

struct imm_frame_epsilon
{
    imm_float loge;
    imm_float log1e;
};

static inline struct imm_frame_epsilon imm_frame_epsilon(imm_float epsilon)
{
    return (struct imm_frame_epsilon){imm_log(epsilon), imm_log(1 - epsilon)};
}

#endif