#include "state/frame.h"
#include "alphabet.h"
#include "logaddexp.h"
#include "report.h"
#include <math.h>
#include <stdlib.h>

struct frame_state
{
    double *base_emiss_lprobs;
    struct nhmm_codon *codon;
    double epsilon;
    double leps;
    double l1eps;
};

double joint_seq_len1(const struct nhmm_state *state, const char *seq);
double joint_seq_len2(const struct nhmm_state *state, const char *seq);
double joint_seq_len3(const struct nhmm_state *state, const char *seq);
double joint_seq_len4(const struct nhmm_state *state, const char *seq);
double joint_seq_len5(const struct nhmm_state *state, const char *seq);
double codon_lprob(const struct nhmm_state *state, const char *codon);
double base_lprob(const struct nhmm_state *state, char id);

void frame_state_create(struct nhmm_state *state, const double *base_emiss_lprobs,
                        const struct nhmm_codon *codon, double epsilon)
{
    struct frame_state *s = malloc(sizeof(struct frame_state));

    if (nhmm_alphabet_length(state->alphabet) != 4)
        error("alphabet length is not four");

    s->base_emiss_lprobs = malloc(sizeof(double) * 4);
    memcpy(s->base_emiss_lprobs, base_emiss_lprobs, sizeof(double) * 4);

    s->codon = nhmm_codon_clone(codon);
    s->epsilon = epsilon;
    s->leps = log(epsilon);
    s->l1eps = log(1 - epsilon);

    state->impl = s;
}

double frame_state_emiss_lprob(const struct nhmm_state *state, const char *seq,
                               size_t seq_len)
{
    /* struct frame_state *s = state->impl; */

    if (seq_len == 1)
        return joint_seq_len1(state, seq);
    else if (seq_len == 2)
        return joint_seq_len2(state, seq);
    else if (seq_len == 3)
        return joint_seq_len3(state, seq);
    else if (seq_len == 4)
        return joint_seq_len4(state, seq);
    else if (seq_len == 5)
        return joint_seq_len5(state, seq);

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
    /*     error("zero-probability alphabet"); */ /*     return -1; */
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

    if (s->codon) {
        nhmm_codon_destroy(s->codon);
        s->codon = NULL;
    }

    free(state->impl);
    state->impl = NULL;
}

double joint_seq_len1(const struct nhmm_state *state, const char *seq)
{
    const char codon0AA[3] = {seq[0], NHMM_ANY_SYMBOL, NHMM_ANY_SYMBOL};
    const char codonA0A[3] = {NHMM_ANY_SYMBOL, seq[0], NHMM_ANY_SYMBOL};
    const char codonAA0[3] = {NHMM_ANY_SYMBOL, NHMM_ANY_SYMBOL, seq[0]};

    const struct frame_state *s = state->impl;
    double c = 2 * s->leps + 2 * s->l1eps;

    double e0 = codon_lprob(state, codon0AA);
    double e1 = codon_lprob(state, codonA0A);
    double e2 = codon_lprob(state, codonAA0);
    return c + logaddexp(logaddexp(e0, e1), e2) - log(3);
}

double joint_seq_len2(const struct nhmm_state *state, const char *seq)
{
    const char codonA01[3] = {NHMM_ANY_SYMBOL, seq[0], seq[1]};
    const char codon0A1[3] = {seq[0], NHMM_ANY_SYMBOL, seq[1]};
    const char codon01A[3] = {seq[0], seq[1], NHMM_ANY_SYMBOL};

    const char codon0AA[3] = {seq[0], NHMM_ANY_SYMBOL, NHMM_ANY_SYMBOL};
    const char codonA0A[3] = {NHMM_ANY_SYMBOL, seq[0], NHMM_ANY_SYMBOL};
    const char codonAA0[3] = {NHMM_ANY_SYMBOL, NHMM_ANY_SYMBOL, seq[0]};

    const char codon1AA[3] = {seq[1], NHMM_ANY_SYMBOL, NHMM_ANY_SYMBOL};
    const char codonA1A[3] = {NHMM_ANY_SYMBOL, seq[1], NHMM_ANY_SYMBOL};
    const char codonAA1[3] = {NHMM_ANY_SYMBOL, NHMM_ANY_SYMBOL, seq[1]};

    const struct frame_state *s = state->impl;

    double c = log(2) + s->leps + s->l1eps * 3 - log(3);
    double eA01 = codon_lprob(state, codonA01);
    double e0A1 = codon_lprob(state, codon0A1);
    double e01A = codon_lprob(state, codon01A);
    double v0 = c + logaddexp(logaddexp(eA01, e0A1), e01A);

    c = 3 * s->leps + s->l1eps - log(3);
    double e0AA = codon_lprob(state, codon0AA);
    double eA0A = codon_lprob(state, codonA0A);
    double eAA0 = codon_lprob(state, codonAA0);
    double v1 = c + logaddexp(logaddexp(e0AA, eA0A), eAA0) + base_lprob(state, seq[1]);

    double e1AA = codon_lprob(state, codon1AA);
    double eA1A = codon_lprob(state, codonA1A);
    double eAA1 = codon_lprob(state, codonAA1);
    double v2 = c + logaddexp(logaddexp(e1AA, eA1A), eAA1) + base_lprob(state, seq[0]);

    return logaddexp(logaddexp(v0, v1), v2);
}

double joint_seq_len3(const struct nhmm_state *state, const char *seq)
{
    /* i = self._base_emission.get */
    /* e = self._codon_prob */
    /* p0 = 4 * self._log1e + e(z1, z2, z3) */

    /* c = LOG(4) + 2 * self._loge + 2 * self._log1e - LOG(9) */
    /* p = [logsumexp([e(_, z2, z3), e(z2, _, z3), e(z2, z3, _)]) + i(z1)] */
    /* p += [logsumexp([e(_, z1, z3), e(z1, _, z3), e(z1, z3, _)]) + i(z2)] */
    /* p += [logsumexp([e(_, z1, z2), e(z1, _, z2), e(z1, z2, _)]) + i(z3)] */
    /* p1 = c + logsumexp(p) */

    /* c = 4 * self._loge - LOG(9) */
    /* p = [logsumexp([e(z3, _, _), e(_, z3, _), e(_, _, z3)]) + i(z1) + i(z2)] */
    /* p += [logsumexp([e(z2, _, _), e(_, z2, _), e(_, _, z2)]) + i(z1) + i(z3)] */
    /* p += [logsumexp([e(z1, _, _), e(_, z1, _), e(_, _, z1)]) + i(z2) + i(z3)] */
    /* p2 = c + logsumexp(p) */

    /* return logsumexp([p0, p1, p2]) */
}

double joint_seq_len4(const struct nhmm_state *state, const char *seq)
{
    /* i = self._base_emission.get */
    /* e = self._codon_prob */

    /* c = self._loge + self._log1e * 3 - LOG(2) */
    /* p = [logsumexp([e(z2, z3, z4) + i(z1), e(z1, z3, z4) + i(z2)])] */
    /* p += [logsumexp([e(z1, z2, z4) + i(z3), e(z1, z2, z3) + i(z4)])] */
    /* p0 = c + logsumexp(p) */

    /* c = 3 * self._loge + self._log1e - LOG(9) */
    /* p = [logsumexp([e(_, z3, z4) + i(z1) + i(z2), e(_, z2, z4) + i(z1) + i(z3)])] */
    /* p += [logsumexp([e(_, z2, z3) + i(z1) + i(z4), e(_, z1, z4) + i(z2) + i(z3)])] */
    /* p += [logsumexp([e(_, z1, z3) + i(z2) + i(z4), e(_, z1, z2) + i(z3) + i(z4)])] */
    /* p += [logsumexp([e(z3, _, z4) + i(z1) + i(z2), e(z2, _, z4) + i(z1) + i(z3)])] */
    /* p += [logsumexp([e(z2, _, z3) + i(z1) + i(z4), e(z1, _, z4) + i(z2) + i(z3)])] */
    /* p += [logsumexp([e(z1, _, z3) + i(z2) + i(z4), e(z1, _, z2) + i(z3) + i(z4)])] */
    /* p += [logsumexp([e(z3, z4, _) + i(z1) + i(z2), e(z2, z4, _) + i(z1) + i(z3)])] */
    /* p += [logsumexp([e(z2, z3, _) + i(z1) + i(z4), e(z1, z4, _) + i(z2) + i(z3)])] */
    /* p += [logsumexp([e(z1, z3, _) + i(z2) + i(z4), e(z1, z2, _) + i(z3) + i(z4)])] */
    /* p1 = c + logsumexp(p) */

    /* return logsumexp([p0, p1]) */
}

double joint_seq_len5(const struct nhmm_state *state, const char *seq)
{
    /* i = self._base_emission.get */
    /* e = self._codon_prob */

    /* c = 2 * self._loge + 2 * self._log1e - LOG(10) */
    /* p = [logsumexp([i(z1) + i(z2) + e(z3, z4, z5), i(z1) + i(z3) + e(z2, z4, z5)])]
     */
    /* p += [logsumexp([i(z1) + i(z4) + e(z2, z3, z5), i(z1) + i(z5) + e(z2, z3, z4)])]
     */
    /* p += [logsumexp([i(z2) + i(z3) + e(z1, z4, z5), i(z2) + i(z4) + e(z1, z3, z5)])]
     */
    /* p += [logsumexp([i(z2) + i(z5) + e(z1, z3, z4), i(z3) + i(z4) + e(z1, z2, z5)])]
     */
    /* p += [logsumexp([i(z3) + i(z5) + e(z1, z2, z4), i(z4) + i(z5) + e(z1, z2, z3)])]
     */

    /* return c + logsumexp(p) */
}

double codon_lprob(const struct nhmm_state *state, const char *codon)
{
    const struct nhmm_alphabet *alphabet = nhmm_state_get_alphabet(state);
    double lprob = -INFINITY;
    int bases_idx[3 * 4] = {0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3};
    size_t nbases[3] = {4, 4, 4};

    for (size_t i = 0; i < 3; ++i) {
        if (codon[i] != NHMM_ANY_SYMBOL) {
            bases_idx[i * 4] = alphabet_symbol_idx(alphabet, codon[i]);
            nbases[i] = 1;
        }
    }

    const int *a_idx = bases_idx;
    const int *b_idx = bases_idx + 4;
    const int *c_idx = bases_idx + 8;
    const struct frame_state *s = state->impl;
    for (size_t a = 0; a < nbases[0]; ++a) {
        for (size_t b = 0; b < nbases[1]; ++b) {
            for (size_t c = 0; c < nbases[2]; ++c) {
                lprob = logaddexp(lprob, nhmm_codon_get_lprob(s->codon, a_idx[a],
                                                              b_idx[b], c_idx[c]));
            }
        }
    }

    return lprob;
}

double base_lprob(const struct nhmm_state *state, char id)
{
    int idx = alphabet_symbol_idx(nhmm_state_get_alphabet(state), id);
    const struct frame_state *s = state->impl;
    return s->base_emiss_lprobs[(size_t)idx];
}

