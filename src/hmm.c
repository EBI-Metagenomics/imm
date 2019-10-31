#include "dp.h"
#include "hide.h"
#include "imm/imm.h"
#include "mstate.h"
#include "mstate_sort.h"
#include "mtrans.h"
#include <stdlib.h>
#include <string.h>

struct imm_hmm
{
    struct imm_abc const* abc;

    struct mstate* mm_states;
};

static double hmm_start_lprob(struct imm_hmm const* hmm, struct imm_state const* state);
static int    hmm_normalize_trans(struct mstate* mm_state);
static int    abc_has_symbols(struct imm_abc const* abc, char const* seq, int seq_len);

struct imm_hmm* imm_hmm_create(struct imm_abc const* abc)
{
    struct imm_hmm* hmm = malloc(sizeof(struct imm_hmm));
    hmm->abc = abc;
    imm_mstate_create(&hmm->mm_states);
    return hmm;
}

void imm_hmm_destroy(struct imm_hmm* hmm)
{
    if (!hmm) {
        imm_error("hmm should not be NULL");
        return;
    }

    imm_mstate_destroy(&hmm->mm_states);

    hmm->abc = NULL;
    free(hmm);
}

int imm_hmm_add_state(struct imm_hmm* hmm, struct imm_state const* state, double start_lprob)
{
    if (!state) {
        imm_error("state cannot be NULL");
        return 1;
    }

    if (imm_mstate_find(hmm->mm_states, state) != NULL) {
        imm_error("state already exists");
        return 1;
    }

    imm_mstate_add_state(&hmm->mm_states, state, start_lprob);
    return 0;
}

int imm_hmm_del_state(struct imm_hmm* hmm, struct imm_state const* state)
{
    if (imm_mstate_del_state(&hmm->mm_states, state)) {
        imm_error("state not found");
        return 1;
    }

    struct mstate* curr = hmm->mm_states;
    while (curr) {
        imm_mstate_del_trans(curr, state);
        curr = imm_mstate_next(curr);
    }

    return 0;
}

int imm_hmm_set_start(struct imm_hmm* hmm, struct imm_state const* state, double lprob)
{
    struct mstate* mm_state = imm_mstate_find(hmm->mm_states, state);
    if (!mm_state)
        return 1;

    imm_mstate_set_start(mm_state, lprob);
    return 0;
}

int imm_hmm_set_trans(struct imm_hmm* hmm, struct imm_state const* src_state,
                      struct imm_state const* dst_state, double lprob)
{
    struct mstate* src = imm_mstate_find(hmm->mm_states, src_state);
    if (!src) {
        imm_error("source state not found");
        return 1;
    }

    if (!imm_mstate_find_c(hmm->mm_states, dst_state)) {
        imm_error("destination state not found");
        return 1;
    }

    if (!imm_lprob_is_valid(lprob)) {
        imm_error("transition probability is invalid");
        return 1;
    }

    struct mm_trans** head_ptr = imm_mstate_get_trans_ptr(src);
    struct mm_trans*  mm_trans = imm_mtrans_find(*head_ptr, dst_state);
    if (mm_trans)
        imm_mtrans_set_lprob(mm_trans, lprob);
    else
        imm_mtrans_add(head_ptr, dst_state, lprob);

    return 0;
}

double imm_hmm_get_trans(struct imm_hmm const* hmm, struct imm_state const* src_state,
                         struct imm_state const* dst_state)
{
    struct mstate const* src = imm_mstate_find_c(hmm->mm_states, src_state);
    if (!src) {
        imm_error("source state not found");
        return imm_lprob_invalid();
    }

    if (!imm_mstate_find_c(hmm->mm_states, dst_state)) {
        imm_error("destination state not found");
        return imm_lprob_invalid();
    }

    struct mm_trans const* mm_trans = imm_mtrans_find_c(imm_mstate_get_trans_c(src), dst_state);
    if (!mm_trans)
        return imm_lprob_zero();

    return imm_mtrans_get_lprob(mm_trans);
}

double imm_hmm_likelihood(struct imm_hmm const* hmm, char const* seq,
                          struct imm_path const* path)
{
    if (!path || !seq) {
        imm_error("path or seq is NULL");
        return imm_lprob_invalid();
    }
    int seq_len = (int)strlen(seq);

    struct imm_step const* step = imm_path_first(path);
    if (!step)
        return imm_lprob_invalid();

    int                     len = imm_step_seq_len(step);
    struct imm_state const* state = imm_step_state(step);

    if (len > seq_len)
        goto len_mismatch;
    if (!state)
        goto not_found_state;

    if (!abc_has_symbols(hmm->abc, seq, seq_len)) {
        imm_error("symbols must belong to alphabet");
        return imm_lprob_invalid();
    }

    double lprob = hmm_start_lprob(hmm, state) + imm_state_lprob(state, seq, len);

    struct imm_state const* prev_state = NULL;

    goto enter;
    while (step) {
        len = imm_step_seq_len(step);
        state = imm_step_state(step);

        if (len > seq_len)
            goto len_mismatch;
        if (!state)
            goto not_found_state;

        lprob += imm_hmm_get_trans(hmm, prev_state, state) + imm_state_lprob(state, seq, len);
        if (!imm_lprob_is_valid(lprob))
            return imm_lprob_invalid();

    enter:
        prev_state = state;
        seq += len;
        seq_len -= len;
        step = imm_path_next(path, step);
    }
    if (seq_len > 0) {
        imm_error("sequence is longer than symbols emitted by path");
        return imm_lprob_invalid();
    }

    return lprob;

len_mismatch:
    imm_error("path emitted more symbols than sequence");
    return imm_lprob_invalid();

not_found_state:
    imm_error("state was not found");
    return imm_lprob_invalid();
}

double imm_hmm_viterbi(struct imm_hmm const* hmm, char const* seq,
                       struct imm_state const* end_state, struct imm_path* path)

{
    if (!hmm || !seq || !end_state) {
        imm_error("viterbi input cannot be NULL");
        return imm_lprob_invalid();
    }

    if (!imm_mstate_find_c(hmm->mm_states, end_state)) {
        imm_error("end_state not found");
        return imm_lprob_invalid();
    }

    int seq_len = (int)strlen(seq);
    if (!abc_has_symbols(hmm->abc, seq, seq_len)) {
        imm_error("symbols must belong to alphabet");
        return imm_lprob_invalid();
    }

    if (seq_len < imm_state_min_seq(end_state)) {
        imm_error("sequence is shorter than end_state's lower bound");
        return imm_lprob_invalid();
    }

    struct mstate const* const* mm_states = imm_mstate_sort(hmm->mm_states);
    if (!mm_states)
        return imm_lprob_invalid();

    struct dp* dp =
        imm_dp_create(mm_states, imm_mstate_nitems(hmm->mm_states), seq, end_state);
    double score = imm_dp_viterbi(dp, path);
    imm_dp_destroy(dp);

    free((struct mstate**)mm_states);
    return score;
}

int imm_hmm_normalize(struct imm_hmm* hmm)
{
    if (imm_hmm_normalize_start(hmm))
        return 1;

    struct mstate* mm_state = hmm->mm_states;
    while (mm_state) {
        if (hmm_normalize_trans(mm_state))
            return 1;
        mm_state = imm_mstate_next(mm_state);
    }
    return 0;
}

int imm_hmm_normalize_start(struct imm_hmm* hmm)
{
    struct mstate const* mm_state = hmm->mm_states;
    if (!mm_state)
        return 0;

    double lnorm = imm_mstate_get_start(mm_state);

    goto enter;
    while (mm_state) {
        lnorm = imm_lprob_add(lnorm, imm_mstate_get_start(mm_state));
    enter:
        mm_state = imm_mstate_next_c(mm_state);
    }

    if (imm_lprob_is_zero(lnorm)) {
        imm_error("no starting state is possible");
        return 1;
    }

    struct mstate* t = hmm->mm_states;
    while (t) {
        imm_mstate_set_start(t, imm_mstate_get_start(t) - lnorm);
        t = imm_mstate_next(t);
    }

    return 0;
}

int imm_hmm_normalize_trans(struct imm_hmm* hmm, struct imm_state const* src)
{
    struct mstate* state = imm_mstate_find(hmm->mm_states, src);
    if (!state) {
        imm_error("source state not found");
        return 1;
    }
    return hmm_normalize_trans(state);
}

static double hmm_start_lprob(struct imm_hmm const* hmm, struct imm_state const* state)
{
    struct mstate const* mm_state = imm_mstate_find(hmm->mm_states, state);
    if (!mm_state) {
        imm_error("state not found");
        return imm_lprob_invalid();
    }
    return imm_mstate_get_start(mm_state);
}

static int hmm_normalize_trans(struct mstate* mm_state)
{
    struct mm_trans const* mm_trans = imm_mstate_get_trans_c(mm_state);
    double                 lnorm = imm_lprob_zero();

    while (mm_trans) {
        lnorm = imm_lprob_add(lnorm, imm_mtrans_get_lprob(mm_trans));
        mm_trans = imm_mtrans_next_c(mm_trans);
    }

    if (imm_lprob_is_zero(lnorm)) {
        imm_error("state has no transition");
        return 1;
    }

    struct mm_trans* t = imm_mstate_get_trans(mm_state);
    while (t) {
        imm_mtrans_set_lprob(t, imm_mtrans_get_lprob(t) - lnorm);
        t = imm_mtrans_next(t);
    }

    return 0;
}

static int abc_has_symbols(struct imm_abc const* abc, char const* seq, int seq_len)
{
    for (int i = 0; i < seq_len; ++i)
        if (!imm_abc_has_symbol(abc, seq[i]))
            return 0;
    return 1;
}
