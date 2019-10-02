#include "state/silent.h"
#include <math.h>

void silent_state_create(struct imm_state *state) { state->impl = NULL; }

double silent_state_emiss_lprob(const struct imm_state *state, const char *seq,
                                size_t seq_len)
{
    if (seq_len == 0)
        return 0.0;
    return -INFINITY;
}

int silent_state_normalize(struct imm_state *state) { return 0; }

size_t silent_state_min_seq(const struct imm_state *state) { return 0; }

size_t silent_state_max_seq(const struct imm_state *state) { return 0; }

void silent_state_destroy(struct imm_state *state) {}
