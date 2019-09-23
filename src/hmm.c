#include "hmm.h"
#include "alphabet.h"
#include "counter.h"
#include "state/state.h"
#include "uthash.h"
#include <stdlib.h>

struct tbl_trans
{
    int dst_state_id;
    double lprob;
    UT_hash_handle hh;
};

void tbl_trans_destroy(struct tbl_trans *tbl_transitions);
struct tbl_trans *tbl_trans_get_trans(struct tbl_trans *tbl_transitions, int dst_state_id);

struct tbl_state
{
    int state_id;
    const struct nhmm_state *state;
    struct tbl_trans *tbl_transitions;
    UT_hash_handle hh;
};

void tbl_state_destroy(struct tbl_state *tbl_states);
struct tbl_state *tbl_state_get_state(struct tbl_state *tbl_states, int state_id);

struct nhmm_hmm *nhmm_hmm_create(const struct nhmm_alphabet *alphabet)
{
    struct nhmm_hmm *hmm = malloc(sizeof(struct nhmm_hmm));
    hmm->alphabet = alphabet;
    hmm->state_id_counter = counter_create();
    hmm->tbl_states = NULL;
    return hmm;
}

int nhmm_hmm_add_state(struct nhmm_hmm *hmm, const struct nhmm_state *state,
                       double start_lprob)
{
    struct tbl_state *tbl_state = malloc(sizeof(struct tbl_state));
    tbl_state->tbl_transitions = NULL;
    tbl_state->state_id = counter_next(hmm->state_id_counter);
    HASH_ADD_INT(hmm->tbl_states, state_id, tbl_state);
    return tbl_state->state_id;
}

int nhmm_hmm_del_state(struct nhmm_hmm *hmm, int state_id)
{
    struct tbl_state *tbl_state = NULL;
    HASH_FIND_INT(hmm->tbl_states, &state_id, tbl_state);
    if (!tbl_state) {
        error("state not found");
        return -1;
    }
    HASH_DEL(hmm->tbl_states, tbl_state);
    return state_id;
}

const struct nhmm_state *nhmm_hmm_get_state(const struct nhmm_hmm *hmm, int state_id)
{
    struct tbl_state *tbl_state = tbl_state_get_state(hmm->tbl_states, state_id);
    if (tbl_state)
        return tbl_state->state;

    error("state not found");
    return NULL;
}

int nhmm_hmm_set_trans(struct nhmm_hmm *hmm, int src_state_id, int dst_state_id,
                       double lprob)
{
    struct tbl_state *src = tbl_state_get_state(hmm->tbl_states, src_state_id);
    if (!src) {
        error("source state not found");
        return -1;
    }

    if (!tbl_state_get_state(hmm->tbl_states, dst_state_id)) {
        error("destination state not found");
        return -1;
    }

    struct tbl_trans *tbl_trans = tbl_trans_get_trans(src->tbl_transitions, dst_state_id);
    if (!tbl_trans)
        HASH_ADD_INT(src->tbl_transitions, dst_state_id, tbl_trans);
    tbl_trans->lprob = lprob;

    return 0;
}

const struct nhmm_alphabet *nhmm_hmm_alphabet(const struct nhmm_hmm *hmm)
{
    return hmm->alphabet;
}

void nhmm_hmm_destroy(struct nhmm_hmm *hmm)
{
    if (!hmm)
        return;

    if (hmm->state_id_counter)
        counter_destroy(hmm->state_id_counter);

    tbl_state_destroy(hmm->tbl_states);

    hmm->alphabet = NULL;
    hmm->state_id_counter = NULL;
    free(hmm);
}

void tbl_state_destroy(struct tbl_state *tbl_states)
{
    struct tbl_state *tbl_state, *tmp;
    if (tbl_states) {
        HASH_ITER(hh, tbl_states, tbl_state, tmp)
        {
            tbl_trans_destroy(tbl_state->tbl_transitions);
            HASH_DEL(tbl_states, tbl_state);
            tbl_state->tbl_transitions = NULL;
            free(tbl_state);
        }
    }
}

struct tbl_state *tbl_state_get_state(struct tbl_state *tbl_states, int state_id)
{
    struct tbl_state *tbl_state = NULL;
    HASH_FIND_INT(tbl_states, &state_id, tbl_state);
    if (tbl_state)
        return tbl_state;
    return NULL;
}

void tbl_trans_destroy(struct tbl_trans *tbl_transitions)
{
    struct tbl_trans *tbl_trans, *tmp;
    if (tbl_transitions) {
        HASH_ITER(hh, tbl_transitions, tbl_trans, tmp)
        {
            HASH_DEL(tbl_transitions, tbl_trans);
            free(tbl_trans);
        }
    }
}

struct tbl_trans *tbl_trans_get_trans(struct tbl_trans *tbl_transitions, int dst_state_id)
{
    struct tbl_trans *tbl_trans = NULL;
    HASH_FIND_INT(tbl_transitions, &dst_state_id, tbl_trans);
    if (tbl_trans)
        return tbl_trans;
    return NULL;
}
