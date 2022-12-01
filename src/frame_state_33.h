#ifndef FRAME_STATE_33_H
#define FRAME_STATE_33_H

#include "imm/float.h"

struct imm_frame_state;
struct imm_seq;

imm_float imm_joint_n3_33(struct imm_frame_state const *,
                          struct imm_seq const *);

#endif
