#include "dp.h"
#include "free.h"
#include "imm/imm.h"
#include "mstate.h"
#include "mstate_sort.h"
#include "mstate_table.h"
#include "mtrans.h"
#include "mtrans_table.h"
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
    hmm->table = mstate_table_create();
    return hmm;
}

void imm_hmm_destroy(struct imm_hmm* hmm)
{
    if (!hmm) {
        imm_error("hmm should not be NULL");
        return;
    }

    mstate_table_destroy(hmm->table);
    free_c(hmm);
}

int imm_hmm_add_state(struct imm_hmm* hmm, struct imm_state const* state, double start_lprob)
{
    if (!state) {
        imm_error("state cannot be NULL");
        return 1;
    }

    unsigned long i = mstate_table_find(hmm->table, state);
    if (i != mstate_table_end(hmm->table)) {
        imm_error("state already exists");
        return 1;
    }

    struct mstate* mstate = mstate_create(state, start_lprob);
    mstate_table_add(hmm->table, mstate);
    return 0;
}

int imm_hmm_del_state(struct imm_hmm* hmm, struct imm_state const* state)
{
    unsigned long i = mstate_table_find(hmm->table, state);
    if (i == mstate_table_end(hmm->table)) {
        imm_error("state not found");
        return 1;
    }

    mstate_table_del(hmm->table, i);
    return 0;
}

int imm_hmm_set_start(struct imm_hmm* hmm, struct imm_state const* state, double lprob)
{
    unsigned long i = mstate_table_find(hmm->table, state);
    if (i == mstate_table_end(hmm->table)) {
        imm_error("state not found");
        return 1;
    }

    mstate_set_start(mstate_table_get(hmm->table, i), lprob);
    return 0;
}

int imm_hmm_set_trans(struct imm_hmm* hmm, struct imm_state const* src_state,
                      struct imm_state const* dst_state, double lprob)
{
    unsigned long src = mstate_table_find(hmm->table, src_state);
    if (src == mstate_table_end(hmm->table)) {
        imm_error("source state not found");
        return 1;
    }

    unsigned long dst = mstate_table_find(hmm->table, dst_state);
    if (dst == mstate_table_end(hmm->table)) {
        imm_error("destination state not found");
        return 1;
    }

    if (!imm_lprob_is_valid(lprob)) {
        imm_error("transition probability is invalid");
        return 1;
    }

    struct mtrans_table* table = mstate_get_mtrans_table(mstate_table_get(hmm->table, src));

    unsigned long i = mtrans_table_find(table, dst_state);
    if (i == mtrans_table_end(table)) {
        imm_error("mtrans_table_add");
        mtrans_table_add(table, mtrans_create(dst_state, lprob));
    } else {
        imm_error("mtrans_set_lprob");
        mtrans_set_lprob(mtrans_table_get(table, i), lprob);
    }

    return 0;
}

double imm_hmm_get_trans(struct imm_hmm const* hmm, struct imm_state const* src_state,
                         struct imm_state const* dst_state)
{
    imm_error("imm_hmm_get_trans: 1");
    unsigned long src = mstate_table_find(hmm->table, src_state);
    if (src == mstate_table_end(hmm->table)) {
        imm_error("source state not found");
        return imm_lprob_invalid();
    }
    imm_error("imm_hmm_get_trans: 2");

    unsigned long dst = mstate_table_find(hmm->table, dst_state);
    if (dst == mstate_table_end(hmm->table)) {
        imm_error("destination state not found");
        return imm_lprob_invalid();
    }
    imm_error("imm_hmm_get_trans: 3 State %s", imm_state_get_name(dst_state));

    struct mtrans_table const* table =
        mstate_get_mtrans_table(mstate_table_get(hmm->table, src));
    unsigned long i = mtrans_table_find(table, dst_state);
    imm_error("i: %ld end: %ld", i, mtrans_table_end(table));

    unsigned long tmp = 0;
    mtrans_table_for_each(tmp, table)
    {
        if (mtrans_table_exist(table, tmp)) {
            imm_error("imm_hmm_get_trans: 3.1 State %s",
                      imm_state_get_name(mtrans_get_state(mtrans_table_get(table, tmp))));
        }
    }

    imm_error("imm_hmm_get_trans: 4 size: %d", mtrans_table_size(table));
    if (i == mtrans_table_end(table))
        return imm_lprob_zero();
    imm_error("imm_hmm_get_trans: 5");

    return mtrans_get_lprob(mtrans_table_get(table, i));
}

double imm_hmm_likelihood(struct imm_hmm const* hmm, char const* seq,
                          struct imm_path const* path)
{
    if (!path || !seq) {
        imm_error("path or seq is NULL");
        return imm_lprob_invalid();
    }
    imm_error("Ponto 2");
    int seq_len = (int)strlen(seq);

    struct imm_step const* step = imm_path_first(path);
    if (!step) {
        imm_error("path must have steps");
        return imm_lprob_invalid();
    }
    imm_error("Ponto 3");

    int                     len = imm_step_seq_len(step);
    struct imm_state const* state = imm_step_state(step);

    if (len > seq_len)
        goto len_mismatch;
    if (!state)
        goto not_found_state;

    imm_error("Ponto 4: State %s", imm_state_get_name(state));
    if (!abc_has_symbols(hmm->abc, seq, seq_len)) {
        imm_error("symbols must belong to alphabet");
        return imm_lprob_invalid();
    }

    double lprob = hmm_start_lprob(hmm, state) + imm_state_lprob(state, seq, len);

    struct imm_state const* prev_state = NULL;
    imm_error("Ponto 5: lprob %f", lprob);

    goto enter;
    while (step) {
        len = imm_step_seq_len(step);
        state = imm_step_state(step);

        imm_error("Ponto 5.1: State %s", imm_state_get_name(state));

        if (len > seq_len)
            goto len_mismatch;
        if (!state)
            goto not_found_state;

        imm_error("Ponto 5.2: trans %f emit %f", imm_hmm_get_trans(hmm, prev_state, state),
                  imm_state_lprob(state, seq, len));
        lprob += imm_hmm_get_trans(hmm, prev_state, state) + imm_state_lprob(state, seq, len);
        if (!imm_lprob_is_valid(lprob))
            return imm_lprob_invalid();

    enter:
        prev_state = state;
        seq += len;
        seq_len -= len;
        step = imm_path_next(path, step);
    }
    imm_error("Ponto 6");
    if (seq_len > 0) {
        imm_error("sequence is longer than symbols emitted by path");
        return imm_lprob_invalid();
    }
    imm_error("Ponto 7");

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
    imm_error("imm_hmm_viterbi: 1");
    if (!hmm || !seq || !end_state || !path) {
        imm_error("viterbi input cannot be NULL");
        return imm_lprob_invalid();
    }
    imm_error("imm_hmm_viterbi: 2");

    unsigned long end = mstate_table_find(hmm->table, end_state);
    if (end == mstate_table_end(hmm->table)) {
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

    imm_error("imm_hmm_viterbi: 3");
    struct mstate const** mstates = mstate_table_array(hmm->table);
    if (mstate_sort(mstates, mstate_table_size(hmm->table))) {
        imm_error("imm_hmm_viterbi: 3.1");
        free_c(mstates);
        return imm_lprob_invalid();
    }

    imm_error("imm_hmm_viterbi: 4");
    struct dp* dp = dp_create(mstates, mstate_table_size(hmm->table), seq, end_state);
    imm_error("imm_hmm_viterbi: 5");
    double score = dp_viterbi(dp, path);
    dp_destroy(dp);

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
        if (mstate_table_exist(hmm->table, i)) {

            if (hmm_normalize_trans(mstate_table_get(hmm->table, i)))
                return 1;
        }
    }
    return 0;
}

int imm_hmm_normalize_start(struct imm_hmm* hmm)
{
    int size = mstate_table_size(hmm->table);
    if (size == 0)
        return 0;

    double* lprobs = malloc(sizeof(double) * (size_t)size);
    double* lprob = lprobs;

    unsigned long i = 0;
    mstate_table_for_each(i, hmm->table)
    {
        if (mstate_table_exist(hmm->table, i)) {
            *lprob = mstate_get_start(mstate_table_get(hmm->table, i));
            ++lprob;
        }
    }

    if (imm_lprob_normalize(lprobs, size)) {
        imm_error("no starting state is possible");
        free_c(lprobs);
        return 1;
    }

    lprob = lprobs;
    mstate_table_for_each(i, hmm->table)
    {
        if (mstate_table_exist(hmm->table, i)) {
            mstate_set_start(mstate_table_get(hmm->table, i), *lprob);
            ++lprob;
        }
    }
    free_c(lprobs);

    return 0;
}

int imm_hmm_normalize_trans(struct imm_hmm* hmm, struct imm_state const* src)
{
    unsigned long i = mstate_table_find(hmm->table, src);
    if (i == mstate_table_end(hmm->table)) {
        imm_error("source state not found");
        return 1;
    }
    return hmm_normalize_trans(mstate_table_get(hmm->table, i));
}

static double hmm_start_lprob(struct imm_hmm const* hmm, struct imm_state const* state)
{
    unsigned long i = mstate_table_find(hmm->table, state);
    if (i == mstate_table_end(hmm->table)) {
        imm_error("state not found");
        return imm_lprob_invalid();
    }
    return mstate_get_start(mstate_table_get(hmm->table, i));
}

static int hmm_normalize_trans(struct mstate* mstate)
{
    struct mtrans_table* table = mstate_get_mtrans_table(mstate);
    int                  size = mtrans_table_size(table);
    if (size == 0)
        return 0;

    double* lprobs = malloc(sizeof(double) * (size_t)size);
    double* lprob = lprobs;

    unsigned long i = 0;
    mtrans_table_for_each(i, table)
    {
        if (mtrans_table_exist(table, i)) {
            *lprob = mtrans_get_lprob(mtrans_table_get(table, i));
            ++lprob;
        }
    }

    if (imm_lprob_normalize(lprobs, size)) {
        imm_error("could not normalize transitions");
        free_c(lprobs);
        return 1;
    }

    lprob = lprobs;
    mtrans_table_for_each(i, table)
    {
        if (mtrans_table_exist(table, i)) {
            mtrans_set_lprob(mtrans_table_get(table, i), *lprob);
            ++lprob;
        }
    }
    free_c(lprobs);

    return 0;
}

static int abc_has_symbols(struct imm_abc const* abc, char const* seq, int seq_len)
{
    for (int i = 0; i < seq_len; ++i)
        if (!imm_abc_has_symbol(abc, seq[i]))
            return 0;
    return 1;
}
