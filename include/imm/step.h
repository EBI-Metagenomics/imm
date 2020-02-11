#ifndef IMM_STEP_H
#define IMM_STEP_H

#include "imm/api.h"

struct imm_step;
struct imm_state;

IMM_API struct imm_step* imm_step_create(struct imm_state const* state, unsigned seq_len);
IMM_API struct imm_state const* imm_step_state(struct imm_step const* step);
IMM_API unsigned                imm_step_seq_len(struct imm_step const* step);
IMM_API void                    imm_step_destroy(struct imm_step const* step);
IMM_API struct imm_step*        imm_step_clone(struct imm_step const* step);

#endif
