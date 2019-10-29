#ifndef IMM_STEP_H
#define IMM_STEP_H

#include "imm/api.h"

struct imm_state;
struct imm_step;

IMM_API struct imm_state const* imm_step_state(struct imm_step const* step);
IMM_API int                     imm_step_seq_len(struct imm_step const* step);

#endif
