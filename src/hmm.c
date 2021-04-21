#include "hmm.h"
#include "dp.h"
#include "std.h"
#include "topological_sort.h"
#include <stdlib.h>

int imm_hmm_add_state(struct imm_hmm* hmm, struct imm_state* state)
{
    if (hash_hashed(&state->hnode)) {
        error("state already belongs to a hmm");
        return IMM_ILLEGALARG;
    }
    hash_add(hmm->state_tbl, &state->hnode, state->id);
    hmm->nstates++;
    return IMM_SUCCESS;
}

struct imm_hmm* imm_hmm_create(struct imm_abc const* abc)
{
    struct imm_hmm* hmm = xmalloc(sizeof(*hmm));
    hmm->abc = abc;
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
    hash_for_each_possible(hmm->state_tbl, cursor, hnode, end_state->id)
    {
        if (cursor == end_state) {
            found = true;
            break;
        }
    }
    if (!found) {
        error("end state not found");
        return NULL;
    }

    struct imm_state** states = xmalloc(sizeof(*states) * hmm->nstates);
    unsigned           bkt = 0;
    unsigned           i = 0;
    hash_for_each(hmm->state_tbl, bkt, cursor, hnode) { states[i++] = cursor; }

    if (topological_sort(states, hmm->nstates, hmm->ntrans)) {
        error("could not sort states");
        free(states);
        return NULL;
    }

    return dp_create(hmm, states, end_state);
}

int imm_hmm_del_state(struct imm_hmm* hmm, struct imm_state* state) { return IMM_NOTIMPLEMENTED; }

void imm_hmm_destroy(struct imm_hmm const* hmm) { free((void*)hmm); }

imm_float imm_hmm_get_start(struct imm_hmm const* hmm, struct imm_state const* state) { return hmm->start_lprob; }

imm_float imm_hmm_get_trans(struct imm_hmm const* hmm, struct imm_state const* src, struct imm_state const* dst)
{
    if (!hash_hashed(&src->hnode) || !hash_hashed(&dst->hnode)) {
        warn("state(s) not found");
        return imm_lprob_invalid();
    }
    struct trans const* trans = hmm_get_trans(hmm, src, dst);
    if (trans)
        return trans->lprob;
    warn("trans not found");
    return imm_lprob_invalid();
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
    if (state != hmm_state(hmm, hmm->start_state)) {
        error("first state is not starting state");
        return imm_lprob_invalid();
    }

    uint_fast32_t remain = imm_seq_length(seq);
    if (step_len > remain)
        goto length_mismatch;

    uint_fast32_t  start = 0;
    struct imm_seq subseq = IMM_SUBSEQ(seq, (uint32_t)start, (uint32_t)step_len);

    imm_float lprob = hmm->start_lprob + imm_state_lprob(state, &subseq);

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

int imm_hmm_normalize_trans(struct imm_hmm const* hmm)
{
    struct imm_state* state = NULL;
    unsigned          bkt = 0;
    int               error = IMM_SUCCESS;
    hash_for_each(hmm->state_tbl, bkt, state, hnode)
    {
        if ((error = imm_hmm_normalize_state_trans(hmm, state)))
            break;
    }
    return error;
}

int imm_hmm_normalize_state_trans(struct imm_hmm const* hmm, struct imm_state* src)
{
    if (!hash_hashed(&src->hnode)) {
        error("state not found");
        return IMM_ILLEGALARG;
    }
    if (stack_empty(&src->trans))
        return IMM_SUCCESS;

    struct trans* trans = NULL;
    struct iter   it = stack_iter(&src->trans);
    imm_float     lnorm = imm_lprob_zero();
    iter_for_each_entry(trans, &it, node) { lnorm = logaddexp(lnorm, trans->lprob); }

    if (!imm_lprob_is_finite(lnorm)) {
        error("non-finite normalization denominator");
        return IMM_ILLEGALARG;
    }

    it = stack_iter(&src->trans);
    iter_for_each_entry(trans, &it, node) { trans->lprob -= lnorm; }
    return IMM_SUCCESS;
}

int imm_hmm_set_start(struct imm_hmm* hmm, struct imm_state const* state, imm_float lprob)
{
    if (!imm_lprob_is_finite(lprob)) {
        error("probability must be finite");
        return IMM_ILLEGALARG;
    }
    if (!hash_hashed(&state->hnode)) {
        error("state not found");
        return IMM_ILLEGALARG;
    }
    hmm->start_lprob = lprob;
    hmm->start_state = state->id;
    return IMM_SUCCESS;
}

int imm_hmm_set_trans(struct imm_hmm* hmm, struct imm_state* src, struct imm_state const* dst, imm_float lprob)
{
    if (!imm_lprob_is_finite(lprob)) {
        error("probability must be finite");
        return IMM_ILLEGALARG;
    }

    if (!hash_hashed(&src->hnode)) {
        error("source state not found");
        return IMM_ILLEGALARG;
    }

    if (!hash_hashed(&dst->hnode)) {
        error("destination state not found");
        return IMM_ILLEGALARG;
    }

    struct trans* trans = hmm_get_trans(hmm, src, dst);

    if (trans) {
        trans->lprob = lprob;
    } else {
        struct trans* newt = hmm->trans + hmm->ntrans++;
        trans_init(newt, src->id, dst->id, lprob);
        hash_add(hmm->trans_tbl, &newt->hnode, newt->pair.key);
        stack_put(&src->trans, &newt->node);
    }

    return IMM_SUCCESS;
}

struct imm_abc const* hmm_abc(struct imm_hmm const* hmm) { return hmm->abc; }

void hmm_add_state(struct imm_hmm* hmm, struct imm_state* state)
{
    hash_add(hmm->state_tbl, &state->hnode, state->id);
    hmm->nstates++;
}

struct imm_state** hmm_get_states(struct imm_hmm const* hmm, struct imm_dp const* dp) { return dp_get_states(dp); }

struct imm_state* hmm_state(struct imm_hmm const* hmm, uint16_t state_id)
{
    struct imm_state* state = NULL;
    hash_for_each_possible(hmm->state_tbl, state, hnode, state_id)
    {
        if (state->id == state_id)
            return state;
    }
    return NULL;
}

struct trans* hmm_get_trans(struct imm_hmm const* hmm, struct imm_state const* src, struct imm_state const* dst)
{
    struct trans*    trans = NULL;
    union state_pair pair = STATE_PAIR_INIT(src->id, dst->id);
    hash_for_each_possible(hmm->trans_tbl, trans, hnode, pair.key)
    {
        if (trans->pair.ids[0] == src->id && trans->pair.ids[1] == dst->id)
            return trans;
    }
    return NULL;
}
