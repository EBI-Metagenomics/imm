#include "imm/step.h"
#include "free.h"
#include "imm/report.h"
#include "imm/state.h"
#include "step_struct.h"

struct imm_step* imm_step_create(struct imm_state const* state, unsigned seq_len)
{
    if (seq_len < imm_state_min_seq(state) || imm_state_max_seq(state) < seq_len) {
        imm_error("seq_len outside the state's range");
        return NULL;
    }
    struct imm_step* step = malloc(sizeof(struct imm_step));
    step->state = state;
    step->seq_len = seq_len;
    return step;
}

struct imm_state const* imm_step_state(struct imm_step const* step) { return step->state; }

unsigned imm_step_seq_len(struct imm_step const* step) { return step->seq_len; }

void imm_step_destroy(struct imm_step const* step) { free_c(step); }

struct imm_step* imm_step_clone(struct imm_step const* step)
{
    struct imm_step* new_step = malloc(sizeof(struct imm_step));
    new_step->state = step->state;
    new_step->seq_len = step->seq_len;
    return new_step;
}
