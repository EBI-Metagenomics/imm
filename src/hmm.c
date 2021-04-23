#include "hmm.h"
#include "common/common.h"
#include "imm/hmm.h"
#include "imm/path.h"
#include "imm/subseq.h"
#include "start.h"

int imm_hmm_add_state(struct imm_hmm *hmm, struct imm_state *state)
{
    if (hash_hashed(&state->hnode))
    {
        error("state already belongs to a hmm");
        return IMM_ILLEGALARG;
    }
    hash_add(hmm->state_tbl, &state->hnode, state->id);
    hmm->nstates++;
    return IMM_SUCCESS;
}

struct imm_hmm *imm_hmm_new(struct imm_abc const *abc)
{
    struct imm_hmm *hmm = xmalloc(sizeof(*hmm));
    hmm->abc = abc;
    start_init(&hmm->start);
    hmm->nstates = 0;
    hash_init(hmm->state_tbl);
    hmm->ntrans = 0;
    hash_init(hmm->trans_tbl);
    return hmm;
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
        error("hmm and seq must have the same alphabet");
        return imm_lprob_invalid();
    }

    uint32_t nsteps = imm_path_nsteps(path);
    if (nsteps == 0)
    {
        error("path must have steps");
        return imm_lprob_invalid();
    }

    uint32_t step_idx = 0;
    struct imm_step const *step = imm_path_step(path, step_idx++);
    unsigned step_len = step->seq_len;
    struct imm_state const *state = hmm_state(hmm, step->state_id);
    if (state != hmm_state(hmm, hmm->start.state_id))
    {
        error("first state must be the starting one");
        return imm_lprob_invalid();
    }

    uint_fast32_t remain = imm_seq_len(seq);
    if (step_len > remain)
        goto length_mismatch;

    uint_fast32_t start = 0;
    struct imm_seq subseq =
        IMM_SUBSEQ(seq, (uint32_t)start, (uint32_t)step_len);

    imm_float lprob = hmm->start.lprob + imm_state_lprob(state, &subseq);

    struct imm_state const *prev_state = NULL;

    goto enter;
    do
    {
        step_len = step->seq_len;
        if (!(state = hmm_state(hmm, step->state_id)))
        {
            error("state not found");
            return imm_lprob_invalid();
        }

        if (step_len > remain)
            goto length_mismatch;

        imm_subseq_init(&subseq, seq, (uint32_t)start, (uint32_t)step_len);

        lprob += imm_hmm_trans(hmm, prev_state, state);
        lprob += imm_state_lprob(state, &subseq);

        if (!imm_lprob_is_valid(lprob))
        {
            error("unexpected invalid floating-point number");
            goto err;
        }

    enter:
        prev_state = state;
        start += step_len;
        BUG(remain < step_len);
        remain -= step_len;
        step = imm_path_step(path, step_idx++);
    } while (step_idx <= nsteps);
    if (remain > 0)
    {
        error("sequence is longer than symbols emitted by path");
        goto err;
    }

    return lprob;

length_mismatch:
    error("path emitted more symbols than sequence");
err:
    return imm_lprob_invalid();
}

int imm_hmm_normalize_trans(struct imm_hmm const *hmm)
{
    struct imm_state *state = NULL;
    unsigned bkt = 0;
    int error = IMM_SUCCESS;
    hash_for_each(hmm->state_tbl, bkt, state, hnode)
    {
        if ((error = imm_hmm_normalize_state_trans(hmm, state)))
            break;
    }
    return error;
}

int imm_hmm_normalize_state_trans(struct imm_hmm const *hmm,
                                  struct imm_state *src)
{
    if (!hash_hashed(&src->hnode))
    {
        error("state not found");
        return IMM_ILLEGALARG;
    }
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
    {
        error("non-finite normalization denominator");
        return IMM_ILLEGALARG;
    }

    it = stack_iter(&src->trans);
    iter_for_each_entry(trans, &it, node) { trans->lprob -= lnorm; }
    return IMM_SUCCESS;
}

int imm_hmm_set_start(struct imm_hmm *hmm, struct imm_state const *state,
                      imm_float lprob)
{
    if (!imm_lprob_is_finite(lprob))
    {
        error("probability must be finite");
        return IMM_ILLEGALARG;
    }
    if (!hash_hashed(&state->hnode))
    {
        error("state not found");
        return IMM_ILLEGALARG;
    }
    hmm->start.lprob = lprob;
    hmm->start.state_id = state->id;
    return IMM_SUCCESS;
}

int imm_hmm_set_trans(struct imm_hmm *hmm, struct imm_state *src,
                      struct imm_state const *dst, imm_float lprob)
{
    if (!imm_lprob_is_finite(lprob))
    {
        error("probability must be finite");
        return IMM_ILLEGALARG;
    }

    if (!hash_hashed(&src->hnode))
    {
        error("source state not found");
        return IMM_ILLEGALARG;
    }

    if (!hash_hashed(&dst->hnode))
    {
        error("destination state not found");
        return IMM_ILLEGALARG;
    }

    struct trans *trans = hmm_trans(hmm, src, dst);

    if (trans)
    {
        trans->lprob = lprob;
    }
    else
    {
        struct trans *newt = hmm->trans + hmm->ntrans++;
        trans_init(newt, src->id, dst->id, lprob);
        hash_add(hmm->trans_tbl, &newt->hnode, newt->pair.key);
        stack_put(&src->trans, &newt->node);
    }

    return IMM_SUCCESS;
}
