#include "hmm.h"
#include "dp.h"
#include "dp_state_table.h"
#include "dp_trans_table.h"
#include "imm/imm.h"
#include "model_state.h"
#include "model_state_sort.h"
#include "model_state_table.h"
#include "model_trans.h"
#include "model_trans_table.h"
#include "seq_code.h"
#include "std.h"
#include <containers/hash.h>
#include <stdlib.h>

static imm_float get_start_lprob(struct imm_hmm const* hmm, struct imm_state const* state);
static int       normalize_transitions(struct model_state* mstate);
static int       normalize_transitions2(struct imm_hmm* hmm);

int imm_hmm_add_state(struct imm_hmm* hmm, struct imm_state* state)
{
    if (hash_hashed(&state->hnode)) {
        error("state already belongs to a hmm");
        return IMM_ILLEGALARG;
    }
    hash_add(hmm->state_tbl, &state->hnode, imm_state_id(state));
    hmm->nstates++;

    unsigned long i = model_state_table_find(hmm->table, state);
    if (i != model_state_table_end(hmm->table)) {
        error("state already exists");
        return IMM_ILLEGALARG;
    }

    struct model_state* mstate = model_state_create(state, imm_lprob_zero());
    model_state_table_add(hmm->table, mstate);
    return IMM_SUCCESS;
}

struct imm_hmm* imm_hmm_create(struct imm_abc const* abc)
{
    struct imm_hmm* hmm = xmalloc(sizeof(*hmm));
    hmm->abc = abc;
    hmm->table = model_state_table_create();
    hmm->start_lprob = imm_lprob_invalid();
    hmm->start_state = UINT16_MAX;
    hmm->nstates = 0;
    hash_init(hmm->state_tbl);
    hmm->ntrans = 0;
    hash_init(hmm->trans_tbl);
    return hmm;
}

struct imm_dp* imm_hmm_create_dp(struct imm_hmm const* hmm, struct imm_state const* end_state)
{
    struct imm_state* cursor = NULL;
    bool              found = false;
    hash_for_each_possible(hmm->state_tbl, cursor, hnode, imm_state_id(end_state))
    {
        if (cursor == end_state) {
            found = true;
            break;
        }
    }
    if (!found) {
        error("end_state not found");
        return NULL;
    }

    struct model_state const** mstates = model_state_table_array(hmm->table);

    if (model_state_topological_sort(mstates, model_state_table_size(hmm->table))) {
        error("could not sort mstates");
        free(mstates);
        return NULL;
    }
    uint16_t nstates = (uint16_t)model_state_table_size(hmm->table);

    return dp_create(hmm->abc, mstates, nstates, end_state);
}

int imm_hmm_del_state(struct imm_hmm* hmm, struct imm_state* state)
{
    /* TODO: not implemented yet */
    return IMM_FAILURE;
    /* unsigned long i = model_state_table_find(hmm->table, state); */

    /* if (!hash_hashed(&state->hnode)) { */
    /*     error("state not found"); */
    /*     return IMM_ILLEGALARG; */
    /* } */

    /* model_state_table_del(hmm->table, i); */
    /* hash_del(&state->hnode); */
    /* hmm->nstates--; */
    /* return IMM_SUCCESS; */
}

void imm_hmm_destroy(struct imm_hmm const* hmm)
{
    if (hmm->table)
        model_state_table_destroy(hmm->table);
    free((void*)hmm);
}

imm_float imm_hmm_get_start(struct imm_hmm const* hmm, struct imm_state const* state) { return hmm->start_lprob; }

imm_float imm_hmm_get_trans(struct imm_hmm const* hmm, struct imm_state const* src_state,
                            struct imm_state const* dst_state)
{

    /* struct trans *cursor = NULL; */
    /* union state_pair pair = STATE_PAIR_INIT(src_state->id, dst_state->id); */
    /* hash_for_each_possible(hmm->trans_tbl, cursor, hnode, pair.key) { */
    /*     if (cursor->state_pair.ids[0] == src_state->id && cursor->state_pair.ids[1] == dst_state->id) { */
    /*         return cursor->lprob; */
    /*     } */
    /* } */

    unsigned long src = model_state_table_find(hmm->table, src_state);
    if (src == model_state_table_end(hmm->table)) {
        error("source state not found");
        return imm_lprob_invalid();
    }

    unsigned long tgt = model_state_table_find(hmm->table, dst_state);
    if (tgt == model_state_table_end(hmm->table)) {
        error("destination state not found");
        return imm_lprob_invalid();
    }

    struct model_trans_table const* table = model_state_get_mtrans_table(model_state_table_get(hmm->table, src));
    unsigned long                   i = model_trans_table_find(table, dst_state);

    if (i == model_trans_table_end(table))
        return imm_lprob_zero();

    return model_trans_get_lprob(model_trans_table_get(table, i));
}

imm_float imm_hmm_loglikelihood(struct imm_hmm const* hmm, struct imm_seq const* seq, struct imm_path const* path)
{
    if (hmm->abc != imm_seq_get_abc(seq)) {
        error("hmm and seq must have the same alphabet");
        return imm_lprob_invalid();
    }

    struct imm_step const* step = imm_path_first(path);
    if (!step) {
        error("path must have steps");
        return imm_lprob_invalid();
    }

    uint_fast8_t            step_len = imm_step_seq_len(step);
    struct imm_state const* state = imm_step_state(step);

    uint_fast32_t remain = imm_seq_length(seq);
    if (step_len > remain)
        goto length_mismatch;

    uint_fast32_t  start = 0;
    struct imm_seq subseq = IMM_SUBSEQ(seq, (uint32_t)start, (uint32_t)step_len);

    imm_float lprob = get_start_lprob(hmm, state) + imm_state_lprob(state, &subseq);

    struct imm_state const* prev_state = NULL;

    goto enter;
    while (step) {
        step_len = imm_step_seq_len(step);
        state = imm_step_state(step);

        if (step_len > remain)
            goto length_mismatch;

        imm_subseq_set(&subseq, seq, (uint32_t)start, (uint32_t)step_len);

        lprob += imm_hmm_get_trans(hmm, prev_state, state);
        lprob += imm_state_lprob(state, &subseq);

        if (!imm_lprob_is_valid(lprob)) {
            error("unexpected invalid floating-point number");
            goto err;
        }

    enter:
        prev_state = state;
        start += step_len;
        BUG(remain < step_len);
        remain -= step_len;
        step = imm_path_next(path, step);
    }
    if (remain > 0) {
        error("sequence is longer than symbols emitted by path");
        goto err;
    }

    return lprob;

length_mismatch:
    error("path emitted more symbols than sequence");

err:
    return imm_lprob_invalid();
}

int imm_hmm_normalize_trans(struct imm_hmm* hmm)
{
    /* return normalize_transitions2(hmm); */

    int err = IMM_SUCCESS;
    /* int err = imm_hmm_normalize_start(hmm); */
    /* if (err) */
    /*     goto err; */

    unsigned long i = 0;
    model_state_table_for_each(i, hmm->table)
    {
        if (model_state_table_exist(hmm->table, i)) {

            err = normalize_transitions(model_state_table_get(hmm->table, i));
            if (err)
                goto err;
        }
    }
    return IMM_SUCCESS;

err:
    error("failed to normalize the hmm");
    return err;
}

int imm_hmm_normalize_state_trans(struct imm_hmm* hmm, struct imm_state const* src_state)
{
    unsigned long i = model_state_table_find(hmm->table, src_state);
    if (i == model_state_table_end(hmm->table)) {
        error("source state not found");
        return IMM_ILLEGALARG;
    }
    return normalize_transitions(model_state_table_get(hmm->table, i));
}

int imm_hmm_set_start(struct imm_hmm* hmm, struct imm_state const* state, imm_float lprob)
{
    if (!imm_lprob_is_finite(lprob)) {
        error("probability must be finite");
        return IMM_ILLEGALARG;
    }

    unsigned long i = model_state_table_find(hmm->table, state);
    if (i == model_state_table_end(hmm->table)) {
        error("state not found");
        return IMM_ILLEGALARG;
    }

    model_state_set_start(model_state_table_get(hmm->table, i), lprob);
    hmm->start_lprob = lprob;
    hmm->start_state = imm_state_id(state);
    return IMM_SUCCESS;
}

int imm_hmm_set_trans(struct imm_hmm* hmm, struct imm_state* src_state, struct imm_state const* tgt_state,
                      imm_float lprob)
{
    if (!imm_lprob_is_finite(lprob)) {
        error("probability must be finite");
        return IMM_ILLEGALARG;
    }

    if (!hash_hashed(&src_state->hnode)) {
        error("source state not found");
        return IMM_ILLEGALARG;
    }
    if (!hash_hashed(&tgt_state->hnode)) {
        error("destination state not found");
        return IMM_ILLEGALARG;
    }

    unsigned long             src = model_state_table_find(hmm->table, src_state);
    struct model_trans_table* table = model_state_get_mtrans_table(model_state_table_get(hmm->table, src));

    unsigned long i = model_trans_table_find(table, tgt_state);
    if (i == model_trans_table_end(table)) {
        model_trans_table_add(table, model_trans_create(tgt_state, lprob));
        struct trans* newt = hmm->trans + hmm->ntrans++;
        trans_init(newt);
        newt->pair.ids[0] = imm_state_id(src_state);
        newt->pair.ids[1] = imm_state_id(tgt_state);
        newt->lprob = lprob;
        hash_add(hmm->trans_tbl, &newt->hnode, newt->pair.key);
        stack_put(&src_state->trans, &newt->node);
    } else {
        model_trans_set_lprob(model_trans_table_get(table, i), lprob);
        union state_pair pair = {.ids[0] = imm_state_id(src_state), .ids[1] = imm_state_id(tgt_state)};
        struct trans*    cursor = NULL;
        hash_for_each_possible(hmm->trans_tbl, cursor, hnode, pair.key)
        {
            if (cursor->pair.key == pair.key) {
                cursor->lprob = lprob;
                return IMM_SUCCESS;
            }
        }
        error("transition not found");
        return IMM_ILLEGALARG;
    }

    return IMM_SUCCESS;
}

struct imm_state const** imm_hmm_states(struct imm_hmm* hmm, uint32_t* nstates)
{
    *nstates = model_state_table_size(hmm->table);
    struct imm_state const** states = xmalloc(*nstates * sizeof(*states));

    unsigned long i = 0;
    size_t        j = 0;
    model_state_table_for_each(i, hmm->table)
    {
        if (model_state_table_exist(hmm->table, i)) {
            states[j++] = model_state_get_state(model_state_table_get(hmm->table, i));
        }
    }
    return states;
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
        error("state not found");
        return imm_lprob_invalid();
    }
    return model_state_get_start(model_state_table_get(hmm->table, i));
}

static int normalize_transitions2(struct imm_hmm* hmm)
{
    struct imm_state* state = NULL;
    unsigned          bkt = 0;
    hash_for_each(hmm->state_tbl, bkt, state, hnode)
    {
        /* TODO: should normalize anyway */
        if (stack_empty(&state->trans))
            continue;
        struct trans* trans = NULL;
        struct iter   it = stack_iter(&state->trans);
        imm_float     lnorm = imm_lprob_zero();
        iter_for_each_entry(trans, &it, node) { lnorm = logaddexp(lnorm, trans->lprob); }

        if (!imm_lprob_is_finite(lnorm)) {
            error("non-finite normalization denominator");
            return IMM_ILLEGALARG;
        }

        iter_for_each_entry(trans, &it, node) { trans->lprob -= lnorm; }
    }

    return IMM_SUCCESS;
}

static int normalize_transitions(struct model_state* mstate)
{
    struct model_trans_table* table = model_state_get_mtrans_table(mstate);
    unsigned                  size = model_trans_table_size(table);
    if (size == 0)
        return IMM_SUCCESS;

    imm_float* lprobs = xmalloc(sizeof(imm_float) * size);
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
        error("could not normalize transitions");
        free(lprobs);
        return IMM_ILLEGALARG;
    }

    lprob = lprobs;
    model_trans_table_for_each(i, table)
    {
        if (model_trans_table_exist(table, i)) {
            model_trans_set_lprob(model_trans_table_get(table, i), *lprob);
            ++lprob;
        }
    }
    free(lprobs);

    return IMM_SUCCESS;
}
