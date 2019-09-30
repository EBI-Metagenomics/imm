#include "counter.h"
#include "logaddexp.h"
#include "nhmm.h"
#include "path.h"
#include "state/state.h"
#include "tbl_state.h"
#include "tbl_trans.h"
#include "utlist.h"
#include <math.h>
#include <stdlib.h>

#define NHMM_API_EXPORTS

struct nhmm_hmm
{
    const struct nhmm_alphabet *alphabet;

    /* Maps `state_id` to `state`. */
    struct tbl_state *tbl_states;
    /* `state_id`s pool: starts with 0, then 1, and so on. */
    struct counter *state_id_counter;
};

double hmm_start_lprob(const struct nhmm_hmm *hmm, int state_id);
int hmm_normalize_start(struct nhmm_hmm *hmm);
int hmm_normalize_trans(struct tbl_state *tbl_state);

NHMM_API struct nhmm_hmm *nhmm_hmm_create(const struct nhmm_alphabet *alphabet)
{
    struct nhmm_hmm *hmm = malloc(sizeof(struct nhmm_hmm));
    hmm->alphabet = alphabet;
    hmm->state_id_counter = counter_create();
    tbl_state_create(&hmm->tbl_states);
    return hmm;
}

NHMM_API int nhmm_hmm_add_state(struct nhmm_hmm *hmm, const struct nhmm_state *state,
                                double start_lprob)
{
    if (!state) {
        error("state cannot be NULL");
        return NHMM_INVALID_STATE_ID;
    }

    int state_id = counter_next(hmm->state_id_counter);
    if (state_id == -1)
        return NHMM_INVALID_STATE_ID;

    tbl_state_add_state(&hmm->tbl_states, state_id, state, start_lprob);
    return state_id;
}

NHMM_API int nhmm_hmm_del_state(struct nhmm_hmm *hmm, int state_id)
{
    if (tbl_state_del_state(&hmm->tbl_states, state_id))
        return -1;

    return 0;
}

NHMM_API const struct nhmm_state *nhmm_hmm_get_state(const struct nhmm_hmm *hmm,
                                                     int state_id)
{
    const struct tbl_state *tbl_state = tbl_state_find(hmm->tbl_states, state_id);
    if (!tbl_state)
        return NULL;

    return tbl_state_get_state(tbl_state);
}

NHMM_API int nhmm_hmm_set_trans(struct nhmm_hmm *hmm, int src_state_id,
                                int dst_state_id, double lprob)
{
    struct tbl_state *src = tbl_state_find(hmm->tbl_states, src_state_id);
    if (!src) {
        error("source state not found");
        return -1;
    }

    if (!tbl_state_find_c(hmm->tbl_states, dst_state_id)) {
        error("destination state not found");
        return -1;
    }

    struct tbl_trans **head_ptr = tbl_state_get_trans_ptr(src);
    struct tbl_trans *tbl_trans = tbl_trans_find(*head_ptr, dst_state_id);
    if (tbl_trans)
        tbl_trans_set_lprob(tbl_trans, lprob);
    else
        tbl_trans_add(head_ptr, dst_state_id, lprob);

    return 0;
}

NHMM_API double nhmm_hmm_get_trans(const struct nhmm_hmm *hmm, int src_state_id,
                                   int dst_state_id)
{
    const struct tbl_state *src = tbl_state_find_c(hmm->tbl_states, src_state_id);
    if (!src) {
        error("source state not found");
        return NAN;
    }

    if (!tbl_state_find_c(hmm->tbl_states, dst_state_id)) {
        error("destination state not found");
        return NAN;
    }

    const struct tbl_trans *tbl_trans =
        tbl_trans_find_c(tbl_state_get_trans_c(src), dst_state_id);
    if (!tbl_trans)
        return -INFINITY;

    return tbl_trans_get_lprob(tbl_trans);
}

NHMM_API double nhmm_hmm_likelihood(const struct nhmm_hmm *hmm, const char *seq,
                                    const struct nhmm_path *path)
{
    if (!path || !seq) {
        error("path or seq is NULL");
        return NAN;
    }
    size_t seq_len = strlen(seq);

    size_t len = path_get_seq_len(path);
    int state_id = path_get_state_id(path);
    const struct nhmm_state *state = nhmm_hmm_get_state(hmm, state_id);

    if (len > seq_len)
        goto len_mismatch;
    if (!state)
        goto not_found_state;

    double lprob =
        hmm_start_lprob(hmm, state_id) + nhmm_state_emiss_lprob(state, seq, len);

    int prev_state_id = NHMM_INVALID_STATE_ID;

    goto enter;
    while (path) {
        len = path_get_seq_len(path);
        state_id = path_get_state_id(path);
        state = nhmm_hmm_get_state(hmm, state_id);

        if (len > seq_len)
            goto len_mismatch;
        if (!state)
            goto not_found_state;

        lprob += nhmm_hmm_get_trans(hmm, prev_state_id, state_id) +
                 nhmm_state_emiss_lprob(state, seq, len);

    enter:
        prev_state_id = state_id;
        seq += len;
        seq_len -= len;
        path = path_next_item(path);
    }
    if (seq_len > 0)
        goto len_mismatch;

    return lprob;

len_mismatch:
    error("path emitted more symbols than sequence");
    return NAN;

not_found_state:
    error("state was not found");
    return NAN;
}

NHMM_API int nhmm_hmm_normalize(struct nhmm_hmm *hmm)
{
    if (hmm_normalize_start(hmm))
        return -1;

    struct tbl_state *tbl_state = hmm->tbl_states;
    while (tbl_state) {
        if (hmm_normalize_trans(tbl_state))
            return -1;
        tbl_state = tbl_state_next(tbl_state);
    }
    return 0;
}

NHMM_API void nhmm_hmm_destroy(struct nhmm_hmm *hmm)
{
    if (!hmm)
        return;

    if (hmm->state_id_counter)
        counter_destroy(hmm->state_id_counter);

    tbl_state_destroy(&hmm->tbl_states);

    hmm->alphabet = NULL;
    hmm->state_id_counter = NULL;
    free(hmm);
}

double hmm_start_lprob(const struct nhmm_hmm *hmm, int state_id)
{
    const struct tbl_state *tbl_state = tbl_state_find(hmm->tbl_states, state_id);
    if (!tbl_state) {
        error("state not found");
        return NAN;
    }
    return tbl_state_get_start_lprob(tbl_state);
}

int hmm_normalize_start(struct nhmm_hmm *hmm)
{
    const struct tbl_state *tbl_state = hmm->tbl_states;
    if (!tbl_state)
        return 0;

    double lnorm = tbl_state_get_start_lprob(tbl_state);

    goto enter;
    while (tbl_state) {
        lnorm = logaddexp(lnorm, tbl_state_get_start_lprob(tbl_state));
    enter:
        tbl_state = tbl_state_next_c(tbl_state);
    }

    if (!isfinite(lnorm)) {
        error("no starting state is possible");
        return -1;
    }

    struct tbl_state *t = hmm->tbl_states;
    while (t) {
        tbl_state_set_start_lprob(t, tbl_state_get_start_lprob(t) - lnorm);
        t = tbl_state_next(t);
    }

    return 0;
}

int hmm_normalize_trans(struct tbl_state *tbl_state)
{
    const struct tbl_trans *tbl_trans = tbl_state_get_trans_c(tbl_state);
    double lnorm = -INFINITY;

    while (tbl_trans) {
        lnorm = logaddexp(lnorm, tbl_trans_get_lprob(tbl_trans));
        tbl_trans = tbl_trans_next_c(tbl_trans);
    }

    if (!isfinite(lnorm)) {
        error("a state has no transition");
        return -1;
    }

    struct tbl_trans *t = tbl_state_get_trans(tbl_state);
    while (t) {
        tbl_trans_set_lprob(t, tbl_trans_get_lprob(t) - lnorm);
        t = tbl_trans_next(t);
    }

    return 0;
}
