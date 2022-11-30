#ifndef FRAME_STATE_24_H
#define FRAME_STATE_24_H

#include "imm/float.h"

struct imm_frame_state;
struct imm_seq;

imm_float imm_joint_n1_24(struct imm_frame_state const *,
                          struct imm_seq const *);
imm_float imm_joint_n2_24(struct imm_frame_state const *,
                          struct imm_seq const *);
imm_float imm_joint_n3_24(struct imm_frame_state const *,
                          struct imm_seq const *);
imm_float imm_joint_n4_24(struct imm_frame_state const *,
                          struct imm_seq const *);
imm_float imm_joint_n5_24(struct imm_frame_state const *,
                          struct imm_seq const *);

#endif
