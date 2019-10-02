#include "state/normal.h"
#include "alphabet.h"
#include "array.h"
#include "report.h"
#include <math.h>
#include <stdlib.h>

struct normal_state
{
    double *emiss_lprobs;
};

void normal_state_create(struct imm_state *state, const double *emiss_lprobs)
{
    struct normal_state *s = malloc(sizeof(struct normal_state));

    size_t length = imm_alphabet_length(state->alphabet);
    s->emiss_lprobs = malloc(sizeof(double) * length);
    memcpy(s->emiss_lprobs, emiss_lprobs, sizeof(double) * length);

    state->impl = s;
}

double normal_state_emiss_lprob(const struct imm_state *state, const char *seq,
                                size_t seq_len)
{
    struct normal_state *s = state->impl;
    if (seq_len == 1) {
        if (alphabet_has_symbol(state->alphabet, seq[0]))
            return s->emiss_lprobs[alphabet_symbol_idx(state->alphabet, seq[0])];
    }
    return -INFINITY;
}

int normal_state_normalize(struct imm_state *state)
{

    size_t len = imm_alphabet_length(imm_state_get_alphabet(state));
    struct normal_state *s = state->impl;
    return log_normalize(s->emiss_lprobs, len);
}

void normal_state_destroy(struct imm_state *state)
{
    if (!state)
        return;

    if (!state->impl)
        return;

    struct normal_state *s = state->impl;
    if (s->emiss_lprobs) {
        free(s->emiss_lprobs);
        s->emiss_lprobs = NULL;
    }

    free(state->impl);
    state->impl = NULL;
}
