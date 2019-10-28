#include "src/imm/step.h"
#include "imm.h"

const struct imm_state* imm_step_state(const struct imm_step* step) { return step->state; }

int imm_step_seq_len(const struct imm_step* step) { return step->seq_len; }
