#include "hmm.h"
#include "dp.h"
#include "dp_state_table.h"
#include "dp_trans_table.h"
#include "free.h"
#include "imm/imm.h"
#include "minmax.h"
#include "model_state.h"
#include "model_state_sort.h"
#include "model_state_table.h"
#include "model_trans.h"
#include "model_trans_table.h"
#include "seq_code.h"
#include <stdlib.h>

struct imm_hmm
{
    struct imm_abc const*     abc;
    struct model_state_table* table;
};

static imm_float get_start_lprob(struct imm_hmm const* hmm, struct imm_state const* state);
static int       normalize_transitions(struct model_state* mstate);

int imm_hmm_add_state(struct imm_hmm* hmm, struct imm_state const* state, imm_float start_lprob)
{
    unsigned long i = model_state_table_find(hmm->table, state);
    if (i != model_state_table_end(hmm->table)) {
        imm_error("state already exists");
        return 1;
    }

    struct model_state* mstate = model_state_create(state, start_lprob);
    model_state_table_add(hmm->table, mstate);
    return 0;
}

struct imm_hmm* imm_hmm_create(struct imm_abc const* abc)
{
    struct imm_hmm* hmm = malloc(sizeof(*hmm));
    hmm->abc = abc;
    hmm->table = model_state_table_create();
    return hmm;
}

struct imm_dp* imm_hmm_create_dp(struct imm_hmm const* hmm, struct imm_state const* end_state)
{
    unsigned long end = model_state_table_find(hmm->table, end_state);
    if (end == model_state_table_end(hmm->table)) {
        imm_error("end_state not found");
        return NULL;
    }

    struct model_state const** mstates = model_state_table_array(hmm->table);

    if (model_state_topological_sort(mstates, model_state_table_size(hmm->table))) {
        imm_error("could not sort mstates");
        free_c(mstates);
        return NULL;
    }
    uint16_t nstates = (uint16_t)model_state_table_size(hmm->table);

    return dp_create(hmm->abc, mstates, nstates, end_state);
}

int imm_hmm_del_state(struct imm_hmm* hmm, struct imm_state const* state)
{
    unsigned long i = model_state_table_find(hmm->table, state);
    if (i == model_state_table_end(hmm->table)) {
        imm_error("state not found");
        return 1;
    }

    model_state_table_del(hmm->table, i);
    return 0;
}

void imm_hmm_destroy(struct imm_hmm const* hmm)
{
    model_state_table_destroy(hmm->table);
    free_c(hmm);
}

imm_float imm_hmm_get_start(struct imm_hmm const* hmm, struct imm_state const* state)
{
    unsigned long i = model_state_table_find(hmm->table, state);
    if (i == model_state_table_end(hmm->table)) {
        imm_error("state not found");
        return imm_lprob_invalid();
    }
    return model_state_get_start(model_state_table_get(hmm->table, i));
}

imm_float imm_hmm_get_trans(struct imm_hmm const* hmm, struct imm_state const* src_state,
                            struct imm_state const* tgt_state)
{
    unsigned long src = model_state_table_find(hmm->table, src_state);
    if (src == model_state_table_end(hmm->table)) {
        imm_error("source state not found");
        return imm_lprob_invalid();
    }

    unsigned long tgt = model_state_table_find(hmm->table, tgt_state);
    if (tgt == model_state_table_end(hmm->table)) {
        imm_error("destination state not found");
        return imm_lprob_invalid();
    }

    struct model_trans_table const* table = model_state_get_mtrans_table(model_state_table_get(hmm->table, src));
    unsigned long                   i = model_trans_table_find(table, tgt_state);

    if (i == model_trans_table_end(table))
        return imm_lprob_zero();

    return model_trans_get_lprob(model_trans_table_get(table, i));
}

imm_float imm_hmm_loglikelihood(struct imm_hmm const* hmm, struct imm_seq const* seq, struct imm_path const* path)
{
    if (hmm->abc != imm_seq_get_abc(seq)) {
        imm_error("hmm and seq must have the same alphabet");
        return imm_lprob_invalid();
    }

    struct imm_step const* step = imm_path_first(path);
    if (!step) {
        imm_error("path must have steps");
        return imm_lprob_invalid();
    }

    unsigned                step_len = imm_step_seq_len(step);
    struct imm_state const* state = imm_step_state(step);

    unsigned remain = imm_seq_length(seq);
    if (step_len > remain)
        goto length_mismatch;

    unsigned start = 0;
    IMM_SUBSEQ(subseq, seq, start, step_len);

    imm_float lprob = get_start_lprob(hmm, state) + imm_state_lprob(state, imm_subseq_cast(&subseq));

    struct imm_state const* prev_state = NULL;

    goto enter;
    while (step) {
        step_len = imm_step_seq_len(step);
        state = imm_step_state(step);

        if (step_len > remain)
            goto length_mismatch;

        imm_subseq_set(&subseq, start, step_len);

        lprob += imm_hmm_get_trans(hmm, prev_state, state);
        lprob += imm_state_lprob(state, imm_subseq_cast(&subseq));

        if (!imm_lprob_is_valid(lprob)) {
            imm_error("unexpected invalid floating-point number");
            goto err;
        }

    enter:
        prev_state = state;
        start += step_len;
        IMM_BUG(remain < step_len);
        remain -= step_len;
        step = imm_path_next(path, step);
    }
    if (remain > 0) {
        imm_error("sequence is longer than symbols emitted by path");
        goto err;
    }

    return lprob;

length_mismatch:
    imm_error("path emitted more symbols than sequence");

err:
    return imm_lprob_invalid();
}

int imm_hmm_normalize(struct imm_hmm* hmm)
{
    if (imm_hmm_normalize_start(hmm))
        goto err;

    unsigned long i = 0;
    model_state_table_for_each(i, hmm->table)
    {
        if (model_state_table_exist(hmm->table, i)) {

            if (normalize_transitions(model_state_table_get(hmm->table, i)))
                goto err;
        }
    }
    return 0;

err:
    imm_error("failed to normalize the hmm");
    return 1;
}

int imm_hmm_normalize_start(struct imm_hmm* hmm)
{
    unsigned size = model_state_table_size(hmm->table);
    if (size == 0)
        return 0;

    imm_float* lprobs = malloc(sizeof(*lprobs) * size);
    imm_float* lprob = lprobs;

    unsigned long i = 0;
    model_state_table_for_each(i, hmm->table)
    {
        if (model_state_table_exist(hmm->table, i)) {
            *lprob = model_state_get_start(model_state_table_get(hmm->table, i));
            ++lprob;
        }
    }

    if (imm_lprob_normalize(lprobs, size)) {
        imm_error("no starting state is possible");
        free_c(lprobs);
        return 1;
    }

    lprob = lprobs;
    model_state_table_for_each(i, hmm->table)
    {
        if (model_state_table_exist(hmm->table, i)) {
            model_state_set_start(model_state_table_get(hmm->table, i), *lprob);
            ++lprob;
        }
    }
    free_c(lprobs);

    return 0;
}

int imm_hmm_normalize_trans(struct imm_hmm* hmm, struct imm_state const* src_state)
{
    unsigned long i = model_state_table_find(hmm->table, src_state);
    if (i == model_state_table_end(hmm->table)) {
        imm_error("source state not found");
        return 1;
    }
    return normalize_transitions(model_state_table_get(hmm->table, i));
}

int imm_hmm_set_start(struct imm_hmm* hmm, struct imm_state const* state, imm_float lprob)
{
    unsigned long i = model_state_table_find(hmm->table, state);
    if (i == model_state_table_end(hmm->table)) {
        imm_error("state not found");
        return 1;
    }

    model_state_set_start(model_state_table_get(hmm->table, i), lprob);
    return 0;
}

int imm_hmm_set_trans(struct imm_hmm* hmm, struct imm_state const* src_state, struct imm_state const* tgt_state,
                      imm_float lprob)
{
    unsigned long src = model_state_table_find(hmm->table, src_state);
    if (src == model_state_table_end(hmm->table)) {
        imm_error("source state not found");
        return 1;
    }

    unsigned long tgt = model_state_table_find(hmm->table, tgt_state);
    if (tgt == model_state_table_end(hmm->table)) {
        imm_error("destination state not found");
        return 1;
    }

    if (!imm_lprob_is_valid(lprob)) {
        imm_error("transition probability is invalid");
        return 1;
    }

    struct model_trans_table* table = model_state_get_mtrans_table(model_state_table_get(hmm->table, src));

    unsigned long i = model_trans_table_find(table, tgt_state);
    if (i == model_trans_table_end(table)) {
        if (!imm_lprob_is_zero(lprob))
            model_trans_table_add(table, model_trans_create(tgt_state, lprob));
    } else {
        if (imm_lprob_is_zero(lprob))
            model_trans_table_del(table, i);
        else
            model_trans_set_lprob(model_trans_table_get(table, i), lprob);
    }

    return 0;
}

struct imm_abc const* hmm_abc(struct imm_hmm const* hmm) { return hmm->abc; }

void hmm_add_mstate(struct imm_hmm* hmm, struct model_state* mstate) { model_state_table_add(hmm->table, mstate); }

struct model_state const* const* hmm_get_mstates(struct imm_hmm const* hmm, struct imm_dp const* dp)
{
    return dp_get_mstates(dp);
}

static imm_float get_start_lprob(struct imm_hmm const* hmm, struct imm_state const* state)
{
    unsigned long i = model_state_table_find(hmm->table, state);
    if (i == model_state_table_end(hmm->table)) {
        imm_error("state not found");
        return imm_lprob_invalid();
    }
    return model_state_get_start(model_state_table_get(hmm->table, i));
}

static int normalize_transitions(struct model_state* mstate)
{
    struct model_trans_table* table = model_state_get_mtrans_table(mstate);
    unsigned                  size = model_trans_table_size(table);
    if (size == 0)
        return 0;

    imm_float* lprobs = malloc(sizeof(imm_float) * (size_t)size);
    imm_float* lprob = lprobs;

    unsigned long i = 0;
    model_trans_table_for_each(i, table)
    {
        if (model_trans_table_exist(table, i)) {
            *lprob = model_trans_get_lprob(model_trans_table_get(table, i));
            ++lprob;
        }
    }

    if (imm_lprob_normalize(lprobs, size)) {
        imm_error("could not normalize transitions");
        free_c(lprobs);
        return 1;
    }

    lprob = lprobs;
    model_trans_table_for_each(i, table)
    {
        if (model_trans_table_exist(table, i)) {
            model_trans_set_lprob(model_trans_table_get(table, i), *lprob);
            ++lprob;
        }
    }
    free_c(lprobs);

    return 0;
}
