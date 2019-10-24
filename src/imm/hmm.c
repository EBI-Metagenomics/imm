#include "imm.h"
#include "src/imm/counter.h"
#include "src/imm/dp.h"
#include "src/imm/hide.h"
#include "src/imm/matrix.h"
#include "src/imm/mm_state.h"
#include "src/imm/mm_state_sort.h"
#include "src/imm/mm_trans.h"
#include "src/imm/path.h"
#include "src/logaddexp/logaddexp.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

struct imm_hmm
{
    const struct imm_abc *abc;

    struct mm_state *mm_states;
};

HIDE double hmm_start_lprob(const struct imm_hmm *hmm, const struct imm_state *state);
HIDE int hmm_normalize_trans(struct mm_state *mm_state);

struct imm_hmm *imm_hmm_create(const struct imm_abc *abc)
{
    struct imm_hmm *hmm = malloc(sizeof(struct imm_hmm));
    hmm->abc = abc;
    mm_state_create(&hmm->mm_states);
    return hmm;
}

int imm_hmm_add_state(struct imm_hmm *hmm, const struct imm_state *state, double start_lprob)
{
    if (!state) {
        imm_error("state cannot be NULL");
        return 1;
    }

    if (mm_state_find(hmm->mm_states, state) != NULL) {
        imm_error("state already exists");
        return 1;
    }

    mm_state_add_state(&hmm->mm_states, state, start_lprob);
    return 0;
}

int imm_hmm_del_state(struct imm_hmm *hmm, const struct imm_state *state)
{
    if (mm_state_del_state(&hmm->mm_states, state)) {
        imm_error("state not found");
        return 1;
    }

    struct mm_state *curr = hmm->mm_states;
    while (curr) {
        mm_state_del_trans(curr, state);
        curr = mm_state_next(curr);
    }

    return 0;
}

int imm_hmm_set_start_lprob(struct imm_hmm *hmm, const struct imm_state *state,
                            double start_lprob)
{
    struct mm_state *mm_state = mm_state_find(hmm->mm_states, state);
    if (!mm_state)
        return 1;

    mm_state_set_start_lprob(mm_state, start_lprob);
    return 0;
}

int imm_hmm_set_trans(struct imm_hmm *hmm, const struct imm_state *src_state,
                      const struct imm_state *dst_state, double lprob)
{
    struct mm_state *src = mm_state_find(hmm->mm_states, src_state);
    if (!src) {
        imm_error("source state not found");
        return 1;
    }

    if (!mm_state_find_c(hmm->mm_states, dst_state)) {
        imm_error("destination state not found");
        return 1;
    }

    if (imm_isnan(lprob)) {
        imm_error("transition probability is invalid");
        return 1;
    }

    struct mm_trans **head_ptr = mm_state_get_trans_ptr(src);
    struct mm_trans *mm_trans = mm_trans_find(*head_ptr, dst_state);
    if (mm_trans)
        mm_trans_set_lprob(mm_trans, lprob);
    else
        mm_trans_add(head_ptr, dst_state, lprob);

    return 0;
}

double imm_hmm_get_trans(const struct imm_hmm *hmm, const struct imm_state *src_state,
                         const struct imm_state *dst_state)
{
    const struct mm_state *src = mm_state_find_c(hmm->mm_states, src_state);
    if (!src) {
        imm_error("source state not found");
        return NAN;
    }

    if (!mm_state_find_c(hmm->mm_states, dst_state)) {
        imm_error("destination state not found");
        return NAN;
    }

    const struct mm_trans *mm_trans = mm_trans_find_c(mm_state_get_trans_c(src), dst_state);
    if (!mm_trans)
        return LOG0;

    return mm_trans_get_lprob(mm_trans);
}

double imm_hmm_likelihood(const struct imm_hmm *hmm, const char *seq,
                          const struct imm_path *path)
{
    if (!path || !seq) {
        imm_error("path or seq is NULL");
        return NAN;
    }
    int seq_len = (int)strlen(seq);

    const struct imm_step *step = path_first_step(path);

    if (step == NULL) {
        return (seq_len == 0) ? 0.0 : -INFINITY;
    }

    int len = path_step_seq_len(step);
    const struct imm_state *state = path_step_state(step);

    if (len > seq_len)
        goto len_mismatch;
    if (!state)
        goto not_found_state;

    double lprob = hmm_start_lprob(hmm, state) + imm_state_lprob(state, seq, len);

    const struct imm_state *prev_state = NULL;

    goto enter;
    while (step) {
        len = path_step_seq_len(step);
        state = path_step_state(step);

        if (len > seq_len)
            goto len_mismatch;
        if (!state)
            goto not_found_state;

        lprob += imm_hmm_get_trans(hmm, prev_state, state) + imm_state_lprob(state, seq, len);

    enter:
        prev_state = state;
        seq += len;
        seq_len -= len;
        step = path_next_step(path, step);
    }
    if (seq_len > 0)
        goto len_mismatch;

    return lprob;

len_mismatch:
    imm_error("path emitted more symbols than sequence");
    return NAN;

not_found_state:
    imm_error("state was not found");
    return NAN;
}

double imm_hmm_viterbi(const struct imm_hmm *hmm, const char *seq,
                       const struct imm_state *end_state, struct imm_path *path)

{
    const struct mm_state *const *mm_states = mm_state_sort(hmm->mm_states);
    if (!mm_states)
        return NAN;

    struct dp *dp = dp_create(mm_states, mm_state_nitems(hmm->mm_states), seq);
    double score = dp_viterbi(dp, end_state, path);
    dp_destroy(dp);

    free((struct mm_state **)mm_states);
    return score;
}

int imm_hmm_normalize(struct imm_hmm *hmm)
{
    if (imm_hmm_normalize_start(hmm))
        return 1;

    struct mm_state *mm_state = hmm->mm_states;
    while (mm_state) {
        if (hmm_normalize_trans(mm_state))
            return 1;
        mm_state = mm_state_next(mm_state);
    }
    return 0;
}

void imm_hmm_destroy(struct imm_hmm *hmm)
{
    if (!hmm)
        return;

    mm_state_destroy(&hmm->mm_states);

    hmm->abc = NULL;
    free(hmm);
}

int imm_hmm_normalize_start(struct imm_hmm *hmm)
{
    const struct mm_state *mm_state = hmm->mm_states;
    if (!mm_state)
        return 0;

    double lnorm = mm_state_get_start_lprob(mm_state);

    goto enter;
    while (mm_state) {
        lnorm = logaddexp(lnorm, mm_state_get_start_lprob(mm_state));
    enter:
        mm_state = mm_state_next_c(mm_state);
    }

    if (!isfinite(lnorm)) {
        imm_error("no starting state is possible");
        return 1;
    }

    struct mm_state *t = hmm->mm_states;
    while (t) {
        mm_state_set_start_lprob(t, mm_state_get_start_lprob(t) - lnorm);
        t = mm_state_next(t);
    }

    return 0;
}

int imm_hmm_normalize_trans(struct imm_hmm *hmm, const struct imm_state *src)
{
    struct mm_state *state = mm_state_find(hmm->mm_states, src);
    if (!state) {
        imm_error("source state not found");
        return 1;
    }
    return hmm_normalize_trans(state);
}

double hmm_start_lprob(const struct imm_hmm *hmm, const struct imm_state *state)
{
    const struct mm_state *mm_state = mm_state_find(hmm->mm_states, state);
    if (!mm_state) {
        imm_error("state not found");
        return NAN;
    }
    return mm_state_get_start_lprob(mm_state);
}

int hmm_normalize_trans(struct mm_state *mm_state)
{
    const struct mm_trans *mm_trans = mm_state_get_trans_c(mm_state);
    double lnorm = LOG0;

    while (mm_trans) {
        lnorm = logaddexp(lnorm, mm_trans_get_lprob(mm_trans));
        mm_trans = mm_trans_next_c(mm_trans);
    }

    if (!isfinite(lnorm)) {
        imm_error("state has no transition");
        return 1;
    }

    struct mm_trans *t = mm_state_get_trans(mm_state);
    while (t) {
        mm_trans_set_lprob(t, mm_trans_get_lprob(t) - lnorm);
        t = mm_trans_next(t);
    }

    return 0;
}
