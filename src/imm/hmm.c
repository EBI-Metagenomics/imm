#include "imm.h"
#include "src/imm/counter.h"
#include "src/imm/dp.h"
#include "src/imm/hide.h"
#include "src/imm/matrix.h"
#include "src/imm/mm_state.h"
#include "src/imm/mm_trans.h"
#include "src/imm/path.h"
#include "src/logaddexp/logaddexp.h"
#include "src/uthash/utlist.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

struct imm_hmm
{
    const struct imm_abc *abc;

    /* Maps `state_id` to `state`. */
    struct mm_state *mm_states;
    /* `state_id`s pool: starts with 0, then 1, and so on. */
    struct counter *state_id_counter;
};

HIDE double hmm_start_lprob(const struct imm_hmm *hmm, int state_id);
HIDE int hmm_normalize_start(struct imm_hmm *hmm);
HIDE int hmm_normalize_trans(struct mm_state *mm_state);

struct imm_hmm *imm_hmm_create(const struct imm_abc *abc)
{
    struct imm_hmm *hmm = malloc(sizeof(struct imm_hmm));
    hmm->abc = abc;
    hmm->state_id_counter = counter_create();
    mm_state_create(&hmm->mm_states);
    return hmm;
}

int imm_hmm_add_state(struct imm_hmm *hmm, const struct imm_state *state, double start_lprob)
{
    if (!state) {
        imm_error("state cannot be NULL");
        return IMM_INVALID_STATE_ID;
    }

    int state_id = counter_next(hmm->state_id_counter);
    if (state_id == -1)
        return IMM_INVALID_STATE_ID;

    mm_state_add_state(&hmm->mm_states, state_id, state, start_lprob);
    return state_id;
}

int imm_hmm_del_state(struct imm_hmm *hmm, int state_id)
{
    if (mm_state_del_state(&hmm->mm_states, state_id))
        return -1;

    return 0;
}

const struct imm_state *imm_hmm_get_state(const struct imm_hmm *hmm, int state_id)
{
    const struct mm_state *mm_state = mm_state_find(hmm->mm_states, state_id);
    if (!mm_state)
        return NULL;

    return mm_state_get_state(mm_state);
}

int imm_hmm_set_start_lprob(struct imm_hmm *hmm, int state_id, double start_lprob)
{
    struct mm_state *mm_state = mm_state_find(hmm->mm_states, state_id);
    if (!mm_state)
        return 1;

    mm_state_set_start_lprob(mm_state, start_lprob);
    return 0;
}

int imm_hmm_set_trans(struct imm_hmm *hmm, int src_state_id, int dst_state_id, double lprob)
{
    struct mm_state *src = mm_state_find(hmm->mm_states, src_state_id);
    if (!src) {
        imm_error("source state not found");
        return -1;
    }

    if (!mm_state_find_c(hmm->mm_states, dst_state_id)) {
        imm_error("destination state not found");
        return -1;
    }

    struct mm_trans **head_ptr = mm_state_get_trans_ptr(src);
    struct mm_trans *mm_trans = mm_trans_find(*head_ptr, dst_state_id);
    if (mm_trans)
        mm_trans_set_lprob(mm_trans, lprob);
    else
        mm_trans_add(head_ptr, dst_state_id, lprob);

    return 0;
}

double imm_hmm_get_trans(const struct imm_hmm *hmm, int src_state_id, int dst_state_id)
{
    const struct mm_state *src = mm_state_find_c(hmm->mm_states, src_state_id);
    if (!src) {
        imm_error("source state not found");
        return NAN;
    }

    if (!mm_state_find_c(hmm->mm_states, dst_state_id)) {
        imm_error("destination state not found");
        return NAN;
    }

    const struct mm_trans *mm_trans =
        mm_trans_find_c(mm_state_get_trans_c(src), dst_state_id);
    if (!mm_trans)
        return -INFINITY;

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

    int len = path_item_seq_len(path);
    int state_id = path_item_state_id(path);
    const struct imm_state *state = imm_hmm_get_state(hmm, state_id);

    if (len > seq_len)
        goto len_mismatch;
    if (!state)
        goto not_found_state;

    double lprob = hmm_start_lprob(hmm, state_id) + imm_state_lprob(state, seq, len);

    int prev_state_id = IMM_INVALID_STATE_ID;

    goto enter;
    while (path) {
        len = path_item_seq_len(path);
        state_id = path_item_state_id(path);
        state = imm_hmm_get_state(hmm, state_id);

        if (len > seq_len)
            goto len_mismatch;
        if (!state)
            goto not_found_state;

        lprob += imm_hmm_get_trans(hmm, prev_state_id, state_id) +
                 imm_state_lprob(state, seq, len);

    enter:
        prev_state_id = state_id;
        seq += len;
        seq_len -= len;
        path = path_next_item(path);
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

double imm_hmm_viterbi(const struct imm_hmm *hmm, const char *seq, int end_state_id)

{
    struct dp *dp = dp_create(hmm->mm_states, seq);
    double score = dp_viterbi(dp, end_state_id);
    dp_destroy(dp);
    return score;
}

int imm_hmm_normalize(struct imm_hmm *hmm)
{
    if (hmm_normalize_start(hmm))
        return -1;

    struct mm_state *mm_state = hmm->mm_states;
    while (mm_state) {
        if (hmm_normalize_trans(mm_state))
            return -1;
        mm_state = mm_state_next(mm_state);
    }
    return 0;
}

void imm_hmm_destroy(struct imm_hmm *hmm)
{
    if (!hmm)
        return;

    if (hmm->state_id_counter)
        counter_destroy(hmm->state_id_counter);

    mm_state_destroy(&hmm->mm_states);

    hmm->abc = NULL;
    hmm->state_id_counter = NULL;
    free(hmm);
}

double hmm_start_lprob(const struct imm_hmm *hmm, int state_id)
{
    const struct mm_state *mm_state = mm_state_find(hmm->mm_states, state_id);
    if (!mm_state) {
        imm_error("state not found");
        return NAN;
    }
    return mm_state_get_start_lprob(mm_state);
}

int hmm_normalize_start(struct imm_hmm *hmm)
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
        return -1;
    }

    struct mm_state *t = hmm->mm_states;
    while (t) {
        mm_state_set_start_lprob(t, mm_state_get_start_lprob(t) - lnorm);
        t = mm_state_next(t);
    }

    return 0;
}

int hmm_normalize_trans(struct mm_state *mm_state)
{
    const struct mm_trans *mm_trans = mm_state_get_trans_c(mm_state);
    double lnorm = -INFINITY;

    while (mm_trans) {
        lnorm = logaddexp(lnorm, mm_trans_get_lprob(mm_trans));
        mm_trans = mm_trans_next_c(mm_trans);
    }

    if (!isfinite(lnorm)) {
        imm_error("a state has no transition");
        return -1;
    }

    struct mm_trans *t = mm_state_get_trans(mm_state);
    while (t) {
        mm_trans_set_lprob(t, mm_trans_get_lprob(t) - lnorm);
        t = mm_trans_next(t);
    }

    return 0;
}
