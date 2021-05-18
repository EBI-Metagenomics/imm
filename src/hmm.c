#include "hmm.h"
#include "dp/dp.h"
#include "imm/dp.h"
#include "imm/hmm.h"
#include "imm/log.h"
#include "imm/lprob.h"
#include "imm/path.h"
#include "imm/state_types.h"
#include "imm/subseq.h"
#include "imm/support.h"
#include "state.h"
#include "support.h"
#include "tsort.h"

static inline void start_init(struct imm_hmm *hmm)
{
    hmm->start.lprob = IMM_LPROB_NAN;
    hmm->start.state_id = IMM_STATE_NULL_ID;
}

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
    hmm->states.size = 0;
    hash_init(hmm->states.tbl);
}

static void init_transitions_table(struct imm_hmm *hmm)
{
    hmm->transitions.size = 0;
    hash_init(hmm->transitions.tbl);
}

static void add_transition(struct imm_hmm *hmm, struct imm_state *src,
                           struct imm_state *dst, imm_float lprob)
{
    IMM_BUG(hmm->transitions.size >= ARRAY_SIZE(hmm->transitions.data));
    struct trans *trans = hmm->transitions.data + hmm->transitions.size++;
    trans_init(trans, src->id, dst->id, lprob);
    hash_add(hmm->transitions.tbl, &trans->hnode, trans->pair.id.key);
    stack_put(&src->trans.outgoing, &trans->outgoing);
    stack_put(&dst->trans.incoming, &trans->incoming);
}

static inline bool has_start_state(struct imm_hmm const *hmm)
{
    return hmm->start.state_id != IMM_STATE_NULL_ID;
}

static void set_state_indices(struct imm_hmm const *hmm,
                              struct imm_state **states)
{
    for (unsigned i = 0; i < hmm->states.size; ++i)
        states[i]->idx = i;

    struct trans *trans = NULL;
    unsigned bkt = 0;
    hash_for_each(hmm->transitions.tbl, bkt, trans, hnode)
    {
        struct imm_state *src = hmm_state(hmm, trans->pair.id.src);
        struct imm_state *dst = hmm_state(hmm, trans->pair.id.dst);
        trans->pair.idx.src = (imm_state_idx_t)src->idx;
        trans->pair.idx.dst = (imm_state_idx_t)dst->idx;
    }
}

int imm_hmm_add_state(struct imm_hmm *hmm, struct imm_state *state)
{
    if (hash_hashed(&state->hnode))
        return error(IMM_ILLEGALARG, "state already belongs to a hmm");
    IMM_BUG(hmm_state(hmm, state->id));
    hmm_add_state(hmm, state);
    return IMM_SUCCESS;
}

struct imm_hmm *imm_hmm_new(struct imm_abc const *abc)
{
    struct imm_hmm *hmm = xmalloc(sizeof(*hmm));
    hmm->abc = abc;
    start_init(hmm);
    init_states_table(hmm);
    init_transitions_table(hmm);
    return hmm;
}

void imm_hmm_del(struct imm_hmm const *hmm) { free((void *)hmm); }

struct imm_dp *imm_hmm_new_dp(struct imm_hmm const *hmm,
                              struct imm_state const *end_state)
{
    struct imm_dp *dp = imm_dp_new(hmm->abc);
    if (imm_hmm_reset_dp(hmm, end_state, dp))
    {
        free(dp);
        return NULL;
    }
    return dp;
}

int imm_hmm_reset_dp(struct imm_hmm const *hmm,
                     struct imm_state const *end_state, struct imm_dp *dp)
{
    int status = IMM_SUCCESS;
    struct imm_state **states = xmalloc(sizeof(*states) * hmm->states.size);

    if (!hmm_state(hmm, end_state->id))
    {
        status = error(IMM_ILLEGALARG, "end state not found");
        goto cleanup;
    }
    if (!has_start_state(hmm))
    {
        status = error(IMM_ILLEGALARG, "start state not found");
        goto cleanup;
    }

    unsigned bkt = 0;
    unsigned i = 0;
    struct imm_state *state = NULL;
    hash_for_each(hmm->states.tbl, bkt, state, hnode) { states[i++] = state; }
    set_state_indices(hmm, states);

    unsigned start_idx = hmm_state(hmm, hmm->start.state_id)->idx;
    if (tsort(hmm->states.size, states, start_idx))
    {
        status = error(IMM_RUNTIMEERROR, "failed to sort states");
        goto cleanup;
    }
    set_state_indices(hmm, states);

    struct dp_args args = dp_args(hmm->transitions.size, hmm->states.size,
                                  states, hmm_state(hmm, hmm->start.state_id),
                                  hmm->start.lprob, end_state);

    dp_reset(dp, &args);

cleanup:
    free(states);
    return status;
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
        warn(IMM_ILLEGALARG, "state(s) not found");
        return imm_lprob_nan();
    }
    struct trans const *trans = hmm_trans(hmm, src, dst);
    if (trans)
        return trans->lprob;
    warn(IMM_ILLEGALARG, "transition not found");
    return imm_lprob_nan();
}

imm_float imm_hmm_loglik(struct imm_hmm const *hmm, struct imm_seq const *seq,
                         struct imm_path const *path)
{
    if (hmm->abc != seq->abc)
    {
        error(IMM_ILLEGALARG, "hmm and seq must have the same alphabet");
        return imm_lprob_nan();
    }

    unsigned nsteps = imm_path_nsteps(path);
    if (nsteps == 0)
    {
        error(IMM_ILLEGALARG, "path must have steps");
        return imm_lprob_nan();
    }

    struct imm_step const *step = imm_path_step(path, 0);
    struct imm_state const *state = hmm_state(hmm, step->state_id);
    if (state != hmm_state(hmm, hmm->start.state_id))
    {
        error(IMM_ILLEGALARG, "first state must be the starting one");
        return imm_lprob_nan();
    }

    if (step->seqlen > imm_seq_size(seq))
    {
        error(IMM_ILLEGALARG, "path emits more symbols than sequence");
        return imm_lprob_nan();
    }

    struct imm_seq subseq = imm_subseq(seq, 0, step->seqlen);
    imm_float lprob = hmm->start.lprob + imm_state_lprob(state, &subseq);

    unsigned start = 0;
    for (unsigned i = 1; i < nsteps; ++i)
    {
        start += step->seqlen;
        step = imm_path_step(path, i);
        if (start + step->seqlen > imm_seq_size(seq))
        {
            error(IMM_ILLEGALARG, "path emits more symbols than sequence");
            return imm_lprob_nan();
        }

        struct imm_state const *prev_state = state;
        if (!(state = hmm_state(hmm, step->state_id)))
        {
            error(IMM_ILLEGALARG, "state not found");
            return imm_lprob_nan();
        }
        imm_subseq_init(&subseq, seq, start, step->seqlen);
        lprob += imm_hmm_trans(hmm, prev_state, state);
        lprob += imm_state_lprob(state, &subseq);
    }

    if (start + step->seqlen < imm_seq_size(seq))
    {
        error(IMM_ILLEGALARG,
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
        return error(IMM_ILLEGALARG, "state not found");

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
        return error(IMM_ILLEGALARG, "non-finite normalization denominator");

    it = stack_iter(&src->trans.outgoing);
    iter_for_each_entry(trans, &it, outgoing) { trans->lprob -= lnorm; }
    return IMM_SUCCESS;
}

int imm_hmm_set_start(struct imm_hmm *hmm, struct imm_state const *state,
                      imm_float lprob)
{
    if (!imm_lprob_is_finite(lprob))
        return error(IMM_ILLEGALARG, "probability must be finite");

    if (!hash_hashed(&state->hnode))
        return error(IMM_ILLEGALARG, "state not found");

    hmm->start.lprob = lprob;
    hmm->start.state_id = state->id;
    return IMM_SUCCESS;
}

int imm_hmm_set_trans(struct imm_hmm *hmm, struct imm_state *src,
                      struct imm_state *dst, imm_float lprob)
{
    if (!imm_lprob_is_finite(lprob))
        return error(IMM_ILLEGALARG, "probability must be finite");

    if (!hash_hashed(&src->hnode))
        return error(IMM_ILLEGALARG, "source state not found");

    if (!hash_hashed(&dst->hnode))
        return error(IMM_ILLEGALARG, "destination state not found");

    struct trans *trans = hmm_trans(hmm, src, dst);

    if (trans)
        trans->lprob = lprob;
    else
        add_transition(hmm, src, dst, lprob);

    return IMM_SUCCESS;
}

struct imm_state *hmm_state(struct imm_hmm const *hmm, unsigned state_id)
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
