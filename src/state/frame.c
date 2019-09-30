#include "state/frame.h"
#include "logaddexp.h"
#include "report.h"
#include <math.h>
#include <stdlib.h>

struct frame_state
{
    double *base_emiss_lprobs;
    double *codon_emiss_lprobs;
    double epsilon;
    double leps;
    double l1eps;
};

void frame_state_create(struct nhmm_state *state, const double *base_emiss_lprobs,
                        const double *codon_emiss_lprobs, double epsilon)
{
    struct frame_state *s = malloc(sizeof(struct frame_state));

    if (nhmm_alphabet_length(state->alphabet) != 4)
        error("alphabet length is not four");

    s->base_emiss_lprobs = malloc(sizeof(double) * 4);
    memcpy(s->base_emiss_lprobs, base_emiss_lprobs, sizeof(double) * 4);

    s->codon_emiss_lprobs = malloc(sizeof(double) * 4 * 4 * 4);
    memcpy(s->codon_emiss_lprobs, codon_emiss_lprobs, sizeof(double) * 4 * 4 * 4);

    s->epsilon = epsilon;
    s->leps = log(epsilon);
    s->l1eps = log(1 - epsilon);

    state->impl = s;
}

double frame_state_emiss_lprob(const struct nhmm_state *state, const char *seq,
                               size_t seq_len)
{
    /* struct frame_state *s = state->impl; */
    /* if (seq_len == 1) { */
    /*     if (alphabet_has_symbol(state->alphabet, seq[0])) */
    /*         return s->emiss_lprobs[alphabet_symbol_idx(state->alphabet, seq[0])]; */
    /* } */
    return -INFINITY;
}

int frame_state_normalize(struct nhmm_state *state)
{
    return 0;
    /* size_t length = nhmm_alphabet_length(nhmm_state_get_alphabet(state)); */
    /* struct frame_state *s = state->impl; */
    /* double lnorm = s->emiss_lprobs[0]; */
    /* for (size_t i = 1; i < length; ++i) */
    /*     lnorm = logaddexp(lnorm, s->emiss_lprobs[i]); */

    /* if (!isfinite(lnorm)) { */
    /*     error("zero-probability alphabet"); */
    /*     return -1; */
    /* } */

    /* for (size_t i = 1; i < length; ++i) */
    /*     s->emiss_lprobs[i] -= lnorm; */

    /* return 0; */
}

void frame_state_destroy(struct nhmm_state *state)
{
    if (!state)
        return;

    if (!state->impl)
        return;

    struct frame_state *s = state->impl;

    if (s->base_emiss_lprobs) {
        free(s->base_emiss_lprobs);
        s->base_emiss_lprobs = NULL;
    }

    if (s->codon_emiss_lprobs) {
        free(s->codon_emiss_lprobs);
        s->codon_emiss_lprobs = NULL;
    }

    free(state->impl);
    state->impl = NULL;
}
