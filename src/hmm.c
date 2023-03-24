#include "imm/hmm.h"
#include "error.h"
#include "imm/dp.h"
#include "imm/dp_args.h"
#include "imm/list.h"
#include "imm/lprob.h"
#include "imm/path.h"
#include "imm/state_types.h"
#include "imm/subseq.h"
#include "state.h"
#include "tsort.h"
#include <assert.h>
#include <stdlib.h>

static inline void start_init(struct imm_hmm *hmm)
{
    hmm->start.lprob = IMM_LPROB_NAN;
    hmm->start.state_id = IMM_STATE_NULL_ID;
}

static void init_states_table(struct imm_hmm *hmm)
{
    hmm->states.size = 0;
    cco_hash_init(hmm->states.tbl);
}

static void init_transitions_table(struct imm_hmm *hmm)
{
    hmm->transitions.size = 0;
    cco_hash_init(hmm->transitions.tbl);
    hmm->transitions.avail = &hmm->transitions.data[0];
}

static int add_transition(struct imm_hmm *hmm, struct imm_state *src,
                          struct imm_state *dst, imm_float lprob)
{
    unsigned max_size = (unsigned)IMM_ARRAY_SIZE(hmm->transitions.data);
    if (hmm->transitions.size >= max_size)
        return error(IMM_TOO_MANY_TRANSITIONS);

    if (hmm->transitions.avail > &hmm->transitions.data[max_size - 1])
        return error(IMM_NOMEM);

    struct imm_trans *trans = hmm->transitions.avail++;
    hmm->transitions.size++;
    trans_init(trans, src->id, dst->id, lprob);
    cco_hash_add(hmm->transitions.tbl, &trans->hnode, trans->pair.id.key);
    imm_stack_put(&trans->outgoing, &src->trans.outgoing);
    imm_stack_put(&trans->incoming, &dst->trans.incoming);
    return IMM_OK;
}

static inline bool has_start_state(struct imm_hmm const *hmm)
{
    return hmm->start.state_id != IMM_STATE_NULL_ID;
}

static struct imm_state *hmm_state(struct imm_hmm const *hmm,
                                   unsigned state_id);

static void set_state_indices(struct imm_hmm const *hmm,
                              struct imm_state **states)
{
    for (unsigned i = 0; i < hmm->states.size; ++i)
        states[i]->idx = i;

    struct imm_trans *trans = NULL;
    unsigned bkt = 0;
    cco_hash_for_each(hmm->transitions.tbl, bkt, trans, hnode)
    {
        struct imm_state *src = hmm_state(hmm, trans->pair.id.src);
        struct imm_state *dst = hmm_state(hmm, trans->pair.id.dst);
        trans->pair.idx.src = (imm_state_idx_t)src->idx;
        trans->pair.idx.dst = (imm_state_idx_t)dst->idx;
    }
}

void imm_hmm_write_dot(struct imm_hmm const *hmm, FILE *restrict fd,
                       imm_state_name *name)
{
    fprintf(fd, "digraph hmm {\n");
    struct imm_trans *t = NULL;
    unsigned bkt = 0;
    cco_hash_for_each(hmm->transitions.tbl, bkt, t, hnode)
    {
        char src_name[IMM_STATE_NAME_SIZE] = {'\0', '\0', '\0', '\0',
                                              '\0', '\0', '\0', '\0'};
        char dst_name[IMM_STATE_NAME_SIZE] = {'\0', '\0', '\0', '\0',
                                              '\0', '\0', '\0', '\0'};
        struct imm_state *src = hmm_state(hmm, t->pair.id.src);
        struct imm_state *dst = hmm_state(hmm, t->pair.id.dst);
        (*name)(src->id, src_name);
        (*name)(dst->id, dst_name);
        fprintf(fd, "%s -> %s [label=%.4f];\n", src_name, dst_name, t->lprob);
    }
    fprintf(fd, "}\n");
}

int imm_hmm_add_state(struct imm_hmm *hmm, struct imm_state *state)
{
    if (cco_hash_hashed(&state->hnode)) return error(IMM_STATE_ALREADY_IN_HMM);
    assert(!hmm_state(hmm, state->id));
    cco_hash_add(hmm->states.tbl, &state->hnode, state->id);
    hmm->states.size++;
    return IMM_OK;
}

static int del_outgoing_transitions(struct imm_hmm *, struct imm_state *);
static int del_incoming_transitions(struct imm_hmm *, struct imm_state *);

int imm_hmm_del_state(struct imm_hmm *hmm, struct imm_state *state)
{
    int rc = IMM_OK;

    if ((rc = del_outgoing_transitions(hmm, state))) return rc;
    if ((rc = del_incoming_transitions(hmm, state))) return rc;

    cco_hash_del(&state->hnode);
    hmm->states.size--;

    return rc;
}

int imm_hmm_init_dp(struct imm_hmm const *hmm,
                    struct imm_state const *end_state, struct imm_dp *dp)
{
    imm_dp_init(dp, hmm->code);
    return imm_hmm_reset_dp(hmm, end_state, dp);
}

void imm_hmm_reset(struct imm_hmm *hmm)
{
    start_init(hmm);
    init_states_table(hmm);
    init_transitions_table(hmm);
}

int imm_hmm_reset_dp(struct imm_hmm const *hmm,
                     struct imm_state const *end_state, struct imm_dp *dp)
{
    int rc = IMM_OK;
    struct imm_state **states =
        malloc(sizeof(struct imm_state *) * hmm->states.size);
    if (!states) return error(IMM_NOMEM);

    if (!hmm_state(hmm, end_state->id))
    {
        rc = error(IMM_END_STATE_NOT_FOUND);
        goto cleanup;
    }
    if (!has_start_state(hmm))
    {
        rc = error(IMM_START_STATE_NOT_FOUND);
        goto cleanup;
    }

    unsigned bkt = 0;
    unsigned i = 0;
    struct imm_state *state = NULL;
    cco_hash_for_each(hmm->states.tbl, bkt, state, hnode)
    {
        states[i++] = state;
    }
    set_state_indices(hmm, states);

    unsigned start_idx = hmm_state(hmm, hmm->start.state_id)->idx;
    if ((rc = imm_tsort(hmm->states.size, states, start_idx))) goto cleanup;
    set_state_indices(hmm, states);

    struct imm_dp_args args = imm_dp_args_init(
        hmm->transitions.size, hmm->states.size, states,
        hmm_state(hmm, hmm->start.state_id), hmm->start.lprob, end_state);

    imm_dp_reset(dp, &args);

cleanup:
    free(states);
    return rc;
}

imm_float imm_hmm_start_lprob(struct imm_hmm const *hmm)
{
    return hmm->start.lprob;
}

static struct imm_trans *hmm_trans(struct imm_hmm const *hmm,
                                   struct imm_state const *src,
                                   struct imm_state const *dst);

imm_float imm_hmm_trans(struct imm_hmm const *hmm, struct imm_state const *src,
                        struct imm_state const *dst)
{
    if (!cco_hash_hashed(&src->hnode) || !cco_hash_hashed(&dst->hnode))
    {
        error(IMM_STATE_NOT_FOUND);
        return imm_lprob_nan();
    }
    struct imm_trans const *trans = hmm_trans(hmm, src, dst);
    if (trans) return trans->lprob;
    error(IMM_TRANSITION_NOT_FOUND);
    return imm_lprob_nan();
}

imm_float imm_hmm_loglik(struct imm_hmm const *hmm, struct imm_seq const *seq,
                         struct imm_path const *path)
{
    if (hmm->code->abc != seq->abc)
    {
        error(IMM_DIFFERENT_ABC);
        return imm_lprob_nan();
    }

    unsigned nsteps = imm_path_nsteps(path);
    if (nsteps == 0)
    {
        error(IMM_PATH_NO_STEPS);
        return imm_lprob_nan();
    }

    struct imm_step const *step = imm_path_step(path, 0);
    struct imm_state const *state = hmm_state(hmm, step->state_id);
    if (state != hmm_state(hmm, hmm->start.state_id))
    {
        error(IMM_FIRST_STATE_NOT_STARTING);
        return imm_lprob_nan();
    }

    if (step->seqlen > imm_seq_size(seq))
    {
        error(IMM_PATH_EMITS_MORE_SEQ);
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
            error(IMM_PATH_EMITS_MORE_SEQ);
            return imm_lprob_nan();
        }

        struct imm_state const *prev_state = state;
        if (!(state = hmm_state(hmm, step->state_id)))
        {
            error(IMM_STATE_NOT_FOUND);
            return imm_lprob_nan();
        }
        imm_subseq_init(&subseq, seq, start, step->seqlen);
        lprob += imm_hmm_trans(hmm, prev_state, state);
        lprob += imm_state_lprob(state, &subseq);
    }

    if (start + step->seqlen < imm_seq_size(seq))
    {
        error(IMM_SEQ_LONGER_THAN_PATH_SYMBOLS);
        return imm_lprob_nan();
    }
    return lprob;
}

int imm_hmm_normalize_trans(struct imm_hmm const *hmm)
{
    struct imm_state *state = NULL;
    unsigned bkt = 0;
    int rc = IMM_OK;
    cco_hash_for_each(hmm->states.tbl, bkt, state, hnode)
    {
        if ((rc = imm_hmm_normalize_state_trans(state))) break;
    }
    return rc;
}

int imm_hmm_normalize_state_trans(struct imm_state *src)
{
    if (!cco_hash_hashed(&src->hnode)) return error(IMM_STATE_NOT_FOUND);

    if (imm_list_empty(&src->trans.outgoing)) return IMM_OK;

    struct imm_trans *trans = NULL;
    imm_float lnorm = imm_lprob_zero();
    imm_list_for_each_entry(trans, &src->trans.outgoing, outgoing)
    {
        lnorm = logaddexp(lnorm, trans->lprob);
    }

    if (!imm_lprob_is_finite(lnorm)) return error(IMM_NON_FINITE_PROBABILITY);

    imm_list_for_each_entry(trans, &src->trans.outgoing, outgoing)
    {
        trans->lprob -= lnorm;
    }
    return IMM_OK;
}

int imm_hmm_set_start(struct imm_hmm *hmm, struct imm_state const *state,
                      imm_float lprob)
{
    if (!imm_lprob_is_finite(lprob)) return error(IMM_NON_FINITE_PROBABILITY);

    if (!cco_hash_hashed(&state->hnode)) return error(IMM_STATE_NOT_FOUND);

    hmm->start.lprob = lprob;
    hmm->start.state_id = state->id;
    return IMM_OK;
}

int imm_hmm_set_trans(struct imm_hmm *hmm, struct imm_state *src,
                      struct imm_state *dst, imm_float lprob)
{
    if (!imm_lprob_is_finite(lprob)) return error(IMM_NON_FINITE_PROBABILITY);

    if (!cco_hash_hashed(&src->hnode)) return error(IMM_STATE_NOT_FOUND);

    if (!cco_hash_hashed(&dst->hnode)) return error(IMM_STATE_NOT_FOUND);

    struct imm_trans *trans = hmm_trans(hmm, src, dst);

    if (trans)
        trans->lprob = lprob;
    else
        return add_transition(hmm, src, dst, lprob);

    return IMM_OK;
}

static struct imm_state *hmm_state(struct imm_hmm const *hmm, unsigned state_id)
{
    struct imm_state *state = NULL;
    cco_hash_for_each_possible(hmm->states.tbl, state, hnode, state_id)
    {
        if (state->id == state_id) return state;
    }
    return NULL;
}

static struct imm_trans *hmm_trans(struct imm_hmm const *hmm,
                                   struct imm_state const *src,
                                   struct imm_state const *dst)
{
    struct imm_trans *trans = NULL;
    struct imm_pair pair = IMM_PAIR_INIT(src->id, dst->id);
    cco_hash_for_each_possible(hmm->transitions.tbl, trans, hnode, pair.id.key)
    {
        if (trans->pair.id.src == src->id && trans->pair.id.dst == dst->id)
            return trans;
    }
    return NULL;
}

static int del_outgoing_transitions(struct imm_hmm *hmm,
                                    struct imm_state *state)
{
    if (!cco_hash_hashed(&state->hnode)) return error(IMM_STATE_NOT_FOUND);
    assert(hmm_state(hmm, state->id));

    struct imm_trans *t = NULL;
    imm_list_for_each_entry(t, &state->trans.outgoing, outgoing)
    {
        struct imm_state *dst = hmm_state(hmm, t->pair.id.dst);
        struct imm_list_head *pos = NULL;
        struct imm_list_head *n = NULL;
        imm_list_for_each_safe(pos, n, &dst->trans.incoming)
        {
            struct imm_trans *x = imm_cof(pos, struct imm_trans, incoming);
            if (x == t)
            {
                imm_list_del(pos);
                break;
            }
        }
        cco_hash_del(&t->hnode);
        hmm->transitions.size--;
    }

    return IMM_OK;
}

static int del_incoming_transitions(struct imm_hmm *hmm,
                                    struct imm_state *state)
{
    if (!cco_hash_hashed(&state->hnode)) return error(IMM_STATE_NOT_FOUND);
    assert(hmm_state(hmm, state->id));

    struct imm_trans *t = NULL;
    imm_list_for_each_entry(t, &state->trans.incoming, incoming)
    {
        struct imm_state *src = hmm_state(hmm, t->pair.id.src);
        struct imm_list_head *pos = NULL;
        struct imm_list_head *n = NULL;
        imm_list_for_each_safe(pos, n, &src->trans.outgoing)
        {
            struct imm_trans *x = imm_cof(pos, struct imm_trans, outgoing);
            if (x == t)
            {
                imm_list_del(pos);
                break;
            }
        }
        cco_hash_del(&t->hnode);
        hmm->transitions.size--;
    }

    return IMM_OK;
}
