#include "dp.h"
#include "free.h"
#include "hide.h"
#include "imm/imm.h"
#include "mstate.h"
#include "mstate_sort.h"
#include "mstate_table.h"
#include "mtrans.h"
#include <stdlib.h>
#include <string.h>

struct imm_hmm
{
    struct imm_abc const* abc;
    struct mstate_table*  table;
};

static double hmm_start_lprob(struct imm_hmm const* hmm, struct imm_state const* state);
static int    hmm_normalize_trans(struct mstate* mstate);
static int    abc_has_symbols(struct imm_abc const* abc, char const* seq, int seq_len);

struct imm_hmm* imm_hmm_create(struct imm_abc const* abc)
{
    struct imm_hmm* hmm = malloc(sizeof(struct imm_hmm));
    hmm->abc = abc;
    hmm->table = imm_mstate_table_create();
    return hmm;
}

void imm_hmm_destroy(struct imm_hmm* hmm)
{
    if (!hmm) {
        imm_error("hmm should not be NULL");
        return;
    }

    imm_mstate_table_destroy(hmm->table);
    free_c(hmm);
}

int imm_hmm_add_state(struct imm_hmm* hmm, struct imm_state const* state, double start_lprob)
{
    if (!state) {
        imm_error("state cannot be NULL");
        return 1;
    }

    unsigned long i = imm_mstate_table_find(hmm->table, state);
    if (i != imm_mstate_table_end(hmm->table)) {
        imm_error("state already exists");
        return 1;
    }

    struct mstate* mstate = imm_mstate_create(state, start_lprob);
    imm_mstate_table_add(hmm->table, mstate);
    return 0;
}

int imm_hmm_del_state(struct imm_hmm* hmm, struct imm_state const* state)
{
    unsigned long i = imm_mstate_table_find(hmm->table, state);
    if (i == imm_mstate_table_end(hmm->table)) {
        imm_error("state not found");
        return 1;
    }

    imm_mstate_table_del(hmm->table, i);
    return 0;
}

int imm_hmm_set_start(struct imm_hmm* hmm, struct imm_state const* state, double lprob)
{
    unsigned long i = imm_mstate_table_find(hmm->table, state);
    if (i == imm_mstate_table_end(hmm->table)) {
        imm_error("state not found");
        return 1;
    }

    imm_mstate_set_start(imm_mstate_table_get(hmm->table, i), lprob);
    return 0;
}

int imm_hmm_set_trans(struct imm_hmm* hmm, struct imm_state const* src_state,
                      struct imm_state const* dst_state, double lprob)
{
    unsigned long src = imm_mstate_table_find(hmm->table, src_state);
    if (src == imm_mstate_table_end(hmm->table)) {
        imm_error("source state not found");
        return 1;
    }

    unsigned long dst = imm_mstate_table_find(hmm->table, dst_state);
    if (dst == imm_mstate_table_end(hmm->table)) {
        imm_error("destination state not found");
        return 1;
    }

    if (!imm_lprob_is_valid(lprob)) {
        imm_error("transition probability is invalid");
        return 1;
    }

    struct mm_trans** head_ptr =
        imm_mstate_get_trans_ptr(imm_mstate_table_get(hmm->table, src));

    struct mm_trans* mm_trans = imm_mtrans_find(*head_ptr, dst_state);
    if (mm_trans)
        imm_mtrans_set_lprob(mm_trans, lprob);
    else
        imm_mtrans_add(head_ptr, dst_state, lprob);

    return 0;
}

double imm_hmm_get_trans(struct imm_hmm const* hmm, struct imm_state const* src_state,
                         struct imm_state const* dst_state)
{
    unsigned long src = imm_mstate_table_find(hmm->table, src_state);
    if (src == imm_mstate_table_end(hmm->table)) {
        imm_error("source state not found");
        return imm_lprob_invalid();
    }

    unsigned long dst = imm_mstate_table_find(hmm->table, dst_state);
    if (dst == imm_mstate_table_end(hmm->table)) {
        imm_error("destination state not found");
        return imm_lprob_invalid();
    }

    struct mm_trans const* mm_trans = imm_mtrans_find_c(
        imm_mstate_get_trans_c(imm_mstate_table_get(hmm->table, src)), dst_state);
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
    if (!step) {
        imm_error("path must have steps");
        return imm_lprob_invalid();
    }

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

    unsigned long end = imm_mstate_table_find(hmm->table, end_state);
    if (end == imm_mstate_table_end(hmm->table)) {
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

    struct mstate const** mstates = imm_mstate_table_array(hmm->table);
    if (imm_mstate_sort(mstates, imm_mstate_table_nitems(hmm->table))) {
        free_c(mstates);
        return imm_lprob_invalid();
    }

    struct dp* dp =
        imm_dp_create(mstates, imm_mstate_table_nitems(hmm->table), seq, end_state);
    double score = imm_dp_viterbi(dp, path);
    imm_dp_destroy(dp);

    free_c(mstates);
    return score;
}

int imm_hmm_normalize(struct imm_hmm* hmm)
{
    if (imm_hmm_normalize_start(hmm))
        return 1;

    unsigned long i = 0;
    mstate_table_for_each(i, hmm->table)
    {
        if (imm_mstate_table_exist(hmm->table, i)) {

            if (hmm_normalize_trans(imm_mstate_table_get(hmm->table, i)))
                return 1;
        }
    }
    return 0;
}

int imm_hmm_normalize_start(struct imm_hmm* hmm)
{
    int size = imm_mstate_table_nitems(hmm->table);
    if (size == 0)
        return 0;

    double* lprobs = malloc(sizeof(double) * (size_t)size);
    double* lprob = lprobs;

    unsigned long i = 0;
    mstate_table_for_each(i, hmm->table)
    {
        if (imm_mstate_table_exist(hmm->table, i)) {
            *lprob = imm_mstate_get_start(imm_mstate_table_get(hmm->table, i));
            ++lprob;
        }
    }

    if (imm_lprob_normalize(lprobs, size)) {
        imm_error("no starting state is possible");
        return 1;
    }

    lprob = lprobs;
    mstate_table_for_each(i, hmm->table)
    {
        if (imm_mstate_table_exist(hmm->table, i)) {
            imm_mstate_set_start(imm_mstate_table_get(hmm->table, i), *lprob);
            ++lprob;
        }
    }
    free_c(lprobs);

    return 0;
}

int imm_hmm_normalize_trans(struct imm_hmm* hmm, struct imm_state const* src)
{
    unsigned long i = imm_mstate_table_find(hmm->table, src);
    if (i == imm_mstate_table_end(hmm->table)) {
        imm_error("source state not found");
        return 1;
    }
    return hmm_normalize_trans(imm_mstate_table_get(hmm->table, i));
}

static double hmm_start_lprob(struct imm_hmm const* hmm, struct imm_state const* state)
{
    unsigned long i = imm_mstate_table_find(hmm->table, state);
    if (i == imm_mstate_table_end(hmm->table)) {
        imm_error("state not found");
        return imm_lprob_invalid();
    }
    return imm_mstate_get_start(imm_mstate_table_get(hmm->table, i));
}

static int hmm_normalize_trans(struct mstate* mstate)
{
    struct mm_trans const* mm_trans = imm_mstate_get_trans_c(mstate);
    double                 lnorm = imm_lprob_zero();

    while (mm_trans) {
        lnorm = imm_lprob_add(lnorm, imm_mtrans_get_lprob(mm_trans));
        mm_trans = imm_mtrans_next_c(mm_trans);
    }

    if (imm_lprob_is_zero(lnorm)) {
        imm_error("state has no transition");
        return 1;
    }

    struct mm_trans* t = imm_mstate_get_trans(mstate);
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
