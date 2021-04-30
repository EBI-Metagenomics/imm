#include "hmm.h"
#include "common/common.h"
#include "dp/dp.h"
#include "imm/error.h"
#include "imm/hmm.h"
#include "imm/lprob.h"
#include "imm/path.h"
#include "imm/state_types.h"
#include "imm/subseq.h"
#include "start.h"
#include "state.h"
#include "tsort.h"
#include <stdint.h>
#include <stdio.h>

static void detach_states(struct imm_hmm *hmm)
{
    unsigned bkt = 0;
    struct imm_state *state = NULL;
    struct hnode *tmp = NULL;
    hash_for_each_safe(hmm->states.tbl, bkt, tmp, state, hnode)
    {
        state_detach(state);
    }
}

static void init_states_table(struct imm_hmm *hmm)
{
    hash_init(hmm->states.tbl);
    hmm->states.size = 0;
}

static void init_transitions_table(struct imm_hmm *hmm)
{
    hmm->transitions.size = 0;
    hash_init(hmm->transitions.tbl);
    hmm->transitions.capacity = sizeof(struct trans) * (1 << 8);
    hmm->transitions.data = xmalloc(hmm->transitions.capacity);
}

static void add_transition(struct imm_hmm *hmm, struct imm_state *src,
                           struct imm_state *dst, imm_float lprob)
{
    size_t size = sizeof(struct trans);
    size_t count = hmm->transitions.size + 1;
    hmm->transitions.data =
        growmem(hmm->transitions.data, count, size, &hmm->transitions.capacity);
    struct trans *trans = hmm->transitions.data + hmm->transitions.size++;
    trans_init(trans, src->id, dst->id, lprob);
    hash_add(hmm->transitions.tbl, &trans->hnode, trans->pair.id.key);
    stack_put(&src->trans.outgoing, &trans->outgoing);
    stack_put(&dst->trans.incoming, &trans->incoming);
}

static void reset_transitions_table(struct imm_hmm *hmm)
{
    hmm->transitions.size = 0;
    hash_init(hmm->transitions.tbl);
}

static inline bool has_start_state(struct imm_hmm const *hmm)
{
    return hmm->start.state_id != IMM_STATE_NULL_ID;
}

int imm_hmm_add_state(struct imm_hmm *hmm, struct imm_state *state)
{
    if (hash_hashed(&state->hnode))
        return xerror(IMM_ILLEGALARG, "state already belongs to a hmm");
    hmm_add_state(hmm, state);
    return IMM_SUCCESS;
}

struct imm_hmm *imm_hmm_new(struct imm_abc const *abc)
{
    struct imm_hmm *hmm = xmalloc(sizeof(*hmm));
    hmm->abc = abc;
    start_init(&hmm->start);
    init_states_table(hmm);
    init_transitions_table(hmm);
    return hmm;
}

void imm_hmm_reset(struct imm_hmm *hmm, struct imm_abc const *abc)
{
    detach_states(hmm);
    hmm->abc = abc;
    start_init(&hmm->start);
    init_states_table(hmm);
    reset_transitions_table(hmm);
}

void imm_hmm_del(struct imm_hmm const *hmm) { free((void *)hmm); }

struct imm_dp *imm_hmm_new_dp(struct imm_hmm const *hmm,
                              struct imm_state const *end_state)
{
    if (!hmm_state(hmm, end_state->id))
    {
        xerror(IMM_ILLEGALARG, "end state not found");
        return NULL;
    }
    if (!has_start_state(hmm))
    {
        xerror(IMM_ILLEGALARG, "start state not found");
        return NULL;
    }

    struct imm_state **states = xmalloc(sizeof(*states) * hmm->states.size);
    unsigned bkt = 0;
    unsigned i = 0;
    struct imm_state *state = NULL;
    hash_for_each(hmm->states.tbl, bkt, state, hnode) { states[i++] = state; }

    if (tsort(states, hmm->states.size, hmm->transitions.size))
    {
        xerror(IMM_RUNTIMEERROR, "failed to sort states");
        free(states);
        return NULL;
    }
    for (i = 0; i < hmm->states.size; ++i)
        states[i]->idx = (imm_state_idx_t)i;

    struct trans *trans = NULL;
    bkt = 0;
    hash_for_each(hmm->transitions.tbl, bkt, trans, hnode)
    {
        struct imm_state *src = hmm_state(hmm, trans->pair.id.src);
        struct imm_state *dst = hmm_state(hmm, trans->pair.id.dst);
        trans->pair.idx.src = src->idx;
        trans->pair.idx.dst = dst->idx;
    }

    struct dp_args args;
    dp_args_init(&args, hmm->transitions.size, hmm->states.size, states,
                 hmm_state(hmm, hmm->start.state_id), hmm->start.lprob,
                 end_state);

    struct imm_dp *dp = dp_new(&args);
    free(states);
    return dp;
}

imm_float imm_hmm_start_lprob(struct imm_hmm const *hmm)
{
    return hmm->start.lprob;
}

imm_float imm_hmm_trans(struct imm_hmm const *hmm, struct imm_state const *src,
                        struct imm_state const *dst)
{
    if (!hash_hashed(&src->hnode) || !hash_hashed(&dst->hnode))
    {
        warn("state(s) not found");
        return imm_lprob_nan();
    }
    struct trans const *trans = hmm_trans(hmm, src, dst);
    if (trans)
        return trans->lprob;
    warn("transition not found");
    return imm_lprob_nan();
}

imm_float imm_hmm_loglik(struct imm_hmm const *hmm, struct imm_seq const *seq,
                         struct imm_path const *path)
{
    if (hmm->abc != seq->abc)
    {
        xerror(IMM_ILLEGALARG, "hmm and seq must have the same alphabet");
        return imm_lprob_nan();
    }

    unsigned nsteps = imm_path_nsteps(path);
    if (nsteps == 0)
    {
        xerror(IMM_ILLEGALARG, "path must have steps");
        return imm_lprob_nan();
    }

    struct imm_step const *step = imm_path_step(path, 0);
    struct imm_state const *state = hmm_state(hmm, step->state_id);
    if (state != hmm_state(hmm, hmm->start.state_id))
    {
        xerror(IMM_ILLEGALARG, "first state must be the starting one");
        return imm_lprob_nan();
    }

    if (step->seqlen > imm_seq_len(seq))
    {
        xerror(IMM_ILLEGALARG, "path emits more symbols than sequence");
        return imm_lprob_nan();
    }

    struct imm_seq subseq = IMM_SUBSEQ(seq, 0, step->seqlen);
    imm_float lprob = hmm->start.lprob + imm_state_lprob(state, &subseq);

    unsigned start = 0;
    for (unsigned i = 1; i < nsteps; ++i)
    {
        start += step->seqlen;
        step = imm_path_step(path, i);
        if (start + step->seqlen > imm_seq_len(seq))
        {
            xerror(IMM_ILLEGALARG, "path emits more symbols than sequence");
            return imm_lprob_nan();
        }

        struct imm_state const *prev_state = state;
        if (!(state = hmm_state(hmm, step->state_id)))
        {
            xerror(IMM_ILLEGALARG, "state not found");
            return imm_lprob_nan();
        }
        imm_subseq_init(&subseq, seq, start, step->seqlen);
        lprob += imm_hmm_trans(hmm, prev_state, state);
        lprob += imm_state_lprob(state, &subseq);
    }

    if (start + step->seqlen < imm_seq_len(seq))
    {
        xerror(IMM_ILLEGALARG,
               "sequence is longer than symbols emitted by path");
        return imm_lprob_nan();
    }
    return lprob;
}

int imm_hmm_normalize_trans(struct imm_hmm const *hmm)
{
    struct imm_state *state = NULL;
    unsigned bkt = 0;
    int err = IMM_SUCCESS;
    hash_for_each(hmm->states.tbl, bkt, state, hnode)
    {
        if ((err = imm_hmm_normalize_state_trans(hmm, state)))
            break;
    }
    return err;
}

int imm_hmm_normalize_state_trans(struct imm_hmm const *hmm,
                                  struct imm_state *src)
{
    if (!hash_hashed(&src->hnode))
        return xerror(IMM_ILLEGALARG, "state not found");

    if (stack_empty(&src->trans.outgoing))
        return IMM_SUCCESS;

    struct trans *trans = NULL;
    struct iter it = stack_iter(&src->trans.outgoing);
    imm_float lnorm = imm_lprob_zero();
    iter_for_each_entry(trans, &it, outgoing)
    {
        lnorm = logaddexp(lnorm, trans->lprob);
    }

    if (!imm_lprob_is_finite(lnorm))
        return xerror(IMM_ILLEGALARG, "non-finite normalization denominator");

    it = stack_iter(&src->trans.outgoing);
    iter_for_each_entry(trans, &it, outgoing) { trans->lprob -= lnorm; }
    return IMM_SUCCESS;
}

int imm_hmm_set_start(struct imm_hmm *hmm, struct imm_state const *state,
                      imm_float lprob)
{
    if (!imm_lprob_is_finite(lprob))
        return xerror(IMM_ILLEGALARG, "probability must be finite");

    if (!hash_hashed(&state->hnode))
        return xerror(IMM_ILLEGALARG, "state not found");

    hmm->start.lprob = lprob;
    hmm->start.state_id = state->id;
    return IMM_SUCCESS;
}

int imm_hmm_set_trans(struct imm_hmm *hmm, struct imm_state *src,
                      struct imm_state *dst, imm_float lprob)
{
    if (!imm_lprob_is_finite(lprob))
        return xerror(IMM_ILLEGALARG, "probability must be finite");

    if (!hash_hashed(&src->hnode))
        return xerror(IMM_ILLEGALARG, "source state not found");

    if (!hash_hashed(&dst->hnode))
        return xerror(IMM_ILLEGALARG, "destination state not found");

    struct trans *trans = hmm_trans(hmm, src, dst);

    if (trans)
        trans->lprob = lprob;
    else
        add_transition(hmm, src, dst, lprob);

    return IMM_SUCCESS;
}

struct imm_state *hmm_state(struct imm_hmm const *hmm, imm_state_id_t state_id)
{
    struct imm_state *state = NULL;
    hash_for_each_possible(hmm->states.tbl, state, hnode, state_id)
    {
        if (state->id == state_id)
            return state;
    }
    return NULL;
}

struct trans *hmm_trans(struct imm_hmm const *hmm, struct imm_state const *src,
                        struct imm_state const *dst)
{
    struct trans *trans = NULL;
    struct pair pair = PAIR_INIT(src->id, dst->id);
    hash_for_each_possible(hmm->transitions.tbl, trans, hnode, pair.id.key)
    {
        if (trans->pair.id.src == src->id && trans->pair.id.dst == dst->id)
            return trans;
    }
    return NULL;
}
