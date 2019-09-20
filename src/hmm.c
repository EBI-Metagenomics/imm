#include "hmm.h"
#include "alphabet.h"
#include "counter.h"
#include "state/state.h"
#include "uthash.h"
#include <stdlib.h>

struct tbl_state
{
    int state_id;
    const struct nhmm_state *state;
    UT_hash_handle hh;
};

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
    struct tbl_state *tbl_state = NULL;
    HASH_FIND_INT(hmm->tbl_states, &state_id, tbl_state);
    if (tbl_state)
        return tbl_state->state;
    return NULL;
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

    struct tbl_state *tbl_state, *tbl_state_tmp;
    if (hmm->tbl_states) {
        HASH_ITER(hh, hmm->tbl_states, tbl_state, tbl_state_tmp)
        {
            HASH_DEL(hmm->tbl_states, tbl_state);
            free(tbl_state);
        }
    }

    hmm->alphabet = NULL;
    hmm->state_id_counter = NULL;
    free(hmm);
}
