#include "hmm.h"
#include "common/common.h"
#include "imm/error.h"
#include "imm/hmm.h"
#include "imm/lprob.h"
#include "imm/path.h"
#include "imm/subseq.h"
#include "start.h"

int imm_hmm_add_state(struct imm_hmm *hmm, struct imm_state *state)
{
    if (hash_hashed(&state->hnode))
        return xerror(IMM_ILLEGALARG, "state already belongs to a hmm");
    hash_add(hmm->state_tbl, &state->hnode, state->id);
    hmm->nstates++;
    return IMM_SUCCESS;
}

struct imm_hmm *imm_hmm_new(struct imm_abc const *abc)
{
    struct imm_hmm *hmm = xmalloc(sizeof(*hmm));
    hash_init(hmm->state_tbl);
    imm_hmm_reset(hmm, abc);
    hmm->trans_capacity = sizeof(*hmm->trans) * (1 << 8);
    hmm->trans = xmalloc(hmm->trans_capacity);
    return hmm;
}

void imm_hmm_reset(struct imm_hmm *hmm, struct imm_abc const *abc)
{
    hmm->abc = abc;
    start_init(&hmm->start);
    hmm->nstates = 0;
    unsigned bkt = 0;
    struct imm_state *state = NULL;
    struct hnode *tmp = NULL;
    hash_for_each_safe(hmm->state_tbl, bkt, tmp, state, hnode)
    {
        hash_del(&state->hnode);
    }
    hash_init(hmm->state_tbl);
    hmm->ntrans = 0;
    hash_init(hmm->trans_tbl);
}

/* struct imm_dp *imm_hmm_create_dp(struct imm_hmm const *hmm, */
/*                                  struct imm_state const *end_state) { */
/*     struct imm_state *cursor = NULL; */
/*     bool found = false; */
/*     hash_for_each_possible(hmm->state_tbl, cursor, hnode, end_state->id) { */
/*         if (cursor == end_state) { */
/*             found = true; */
/*             break; */
/*         } */
/*     } */
/*     if (!found) { */
/*         error("end state not found"); */
/*         return NULL; */
/*     } */

/*     struct imm_state **states = xmalloc(sizeof(*states) * hmm->nstates); */
/*     unsigned bkt = 0; */
/*     unsigned i = 0; */
/*     hash_for_each(hmm->state_tbl, bkt, cursor, hnode) { states[i++] = cursor;
 * } */

/*     if (topological_sort(states, hmm->nstates, hmm->ntrans)) { */
/*         error("could not sort states"); */
/*         free(states); */
/*         return NULL; */
/*     } */

/*     return dp_create(hmm, states, end_state); */
/* } */

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
        return imm_lprob_invalid();
    }
    struct trans const *trans = hmm_trans(hmm, src, dst);
    if (trans)
        return trans->lprob;
    warn("trans not found");
    return imm_lprob_invalid();
}

imm_float imm_hmm_loglik(struct imm_hmm const *hmm, struct imm_seq const *seq,
                         struct imm_path const *path)
{
    if (hmm->abc != seq->abc)
    {
        xerror(IMM_ILLEGALARG, "hmm and seq must have the same alphabet");
        return imm_lprob_invalid();
    }

    uint32_t nsteps = imm_path_nsteps(path);
    if (nsteps == 0)
    {
        xerror(IMM_ILLEGALARG, "path must have steps");
        return imm_lprob_invalid();
    }

    struct imm_step const *step = imm_path_step(path, 0);
    struct imm_state const *state = hmm_state(hmm, step->state_id);
    if (state != hmm_state(hmm, hmm->start.state_id))
    {
        xerror(IMM_ILLEGALARG, "first state must be the starting one");
        return imm_lprob_invalid();
    }

    if (step->seq_len > imm_seq_len(seq))
    {
        xerror(IMM_ILLEGALARG, "path emits more symbols than sequence");
        return imm_lprob_invalid();
    }

    struct imm_seq subseq = IMM_SUBSEQ(seq, 0, step->seq_len);
    imm_float lprob = hmm->start.lprob + imm_state_lprob(state, &subseq);

    uint32_t start = 0;
    for (uint32_t i = 1; i < imm_seq_len(seq); ++i)
    {
        start += step->seq_len;
        step = imm_path_step(path, i);
        if (start + step->seq_len > imm_seq_len(seq))
        {
            xerror(IMM_ILLEGALARG, "path emits more symbols than sequence");
            return imm_lprob_invalid();
        }

        struct imm_state const *prev_state = state;
        if (!(state = hmm_state(hmm, step->state_id)))
        {
            xerror(IMM_ILLEGALARG, "state not found");
            return imm_lprob_invalid();
        }
        imm_subseq_init(&subseq, seq, start, step->seq_len);
        lprob += imm_hmm_trans(hmm, prev_state, state);
        lprob += imm_state_lprob(state, &subseq);
    }

    if (start + step->seq_len < imm_seq_len(seq))
    {
        xerror(IMM_ILLEGALARG,
               "sequence is longer than symbols emitted by path");
        return imm_lprob_invalid();
    }
    return lprob;
}

int imm_hmm_normalize_trans(struct imm_hmm const *hmm)
{
    struct imm_state *state = NULL;
    unsigned bkt = 0;
    int err = IMM_SUCCESS;
    hash_for_each(hmm->state_tbl, bkt, state, hnode)
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

    if (stack_empty(&src->trans))
        return IMM_SUCCESS;

    struct trans *trans = NULL;
    struct iter it = stack_iter(&src->trans);
    imm_float lnorm = imm_lprob_zero();
    iter_for_each_entry(trans, &it, node)
    {
        lnorm = logaddexp(lnorm, trans->lprob);
    }

    if (!imm_lprob_is_finite(lnorm))
        return xerror(IMM_ILLEGALARG, "non-finite normalization denominator");

    it = stack_iter(&src->trans);
    iter_for_each_entry(trans, &it, node) { trans->lprob -= lnorm; }
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
                      struct imm_state const *dst, imm_float lprob)
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
    {
        size_t size = sizeof(*hmm->trans);
        size_t count = hmm->ntrans + 1;
        hmm->trans = growmem(hmm->trans, count, size, &hmm->trans_capacity);
        struct trans *newt = hmm->trans + hmm->ntrans++;
        trans_init(newt, src->id, dst->id, lprob);
        hash_add(hmm->trans_tbl, &newt->hnode, newt->pair.key);
        stack_put(&src->trans, &newt->node);
    }

    return IMM_SUCCESS;
}
