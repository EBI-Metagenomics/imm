#ifndef FRAME_STATE_15_H
#define FRAME_STATE_15_H

#include "imm/float.h"

struct imm_frame_state;
struct imm_seq;

imm_float imm_joint_n1_15(struct imm_frame_state const *,
                          struct imm_seq const *);
imm_float imm_joint_n2_15(struct imm_frame_state const *,
                          struct imm_seq const *);
imm_float imm_joint_n3_15(struct imm_frame_state const *,
                          struct imm_seq const *);
imm_float imm_joint_n4_15(struct imm_frame_state const *,
                          struct imm_seq const *);
imm_float imm_joint_n5_15(struct imm_frame_state const *,
                          struct imm_seq const *);

#endif
