#ifndef IMM_STEP_H
#define IMM_STEP_H

#include "imm/export.h"
#include <stdint.h>

struct imm_state;
struct imm_step;

IMM_API struct imm_step*        imm_step_clone(struct imm_step const* step);
IMM_API struct imm_step*        imm_step_create(struct imm_state const* state, uint8_t seq_len);
IMM_API void                    imm_step_destroy(struct imm_step const* step);
IMM_API uint8_t                 imm_step_seq_len(struct imm_step const* step);
IMM_API struct imm_state const* imm_step_state(struct imm_step const* step);

#endif
