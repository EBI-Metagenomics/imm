#include "step.h"
#include "imm/imm.h"
#include <stdlib.h>

struct imm_step* imm_step_clone(struct imm_step const* step)
{
    struct imm_step* new_step = malloc(sizeof(struct imm_step));
    new_step->state = step->state;
    new_step->seq_len = step->seq_len;
    return new_step;
}

struct imm_step* imm_step_create(struct imm_state const* state, uint8_t seq_len)
{
    if (seq_len < imm_state_min_seq(state) || imm_state_max_seq(state) < seq_len) {
        imm_error("seq_len outside the state's range");
        return NULL;
    }
    struct imm_step* step = malloc(sizeof(*step));
    step->state = state;
    step->seq_len = seq_len;
    return step;
}

void imm_step_destroy(struct imm_step const* step) { free((void*)step); }

uint8_t imm_step_seq_len(struct imm_step const* step) { return step->seq_len; }

struct imm_state const* imm_step_state(struct imm_step const* step) { return step->state; }
