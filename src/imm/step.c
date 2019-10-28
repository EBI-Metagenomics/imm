#include "src/imm/step.h"
#include "imm.h"

struct imm_state const* imm_step_state(struct imm_step const* step) { return step->state; }

int imm_step_seq_len(struct imm_step const* step) { return step->seq_len; }
