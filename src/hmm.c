#include "counter.h"
#include "nhmm.h"
#include "path.h"
#include "state/state.h"
#include "tbl_state.h"
#include "utlist.h"
#include <math.h>
#include <stdlib.h>

struct nhmm_hmm
{
    const struct nhmm_alphabet *alphabet;

    /* Maps `state_id` to `state`. */
    struct tbl_state *tbl_states;
    /* `state_id`s pool: starts with 0, then 1, and so on. */
    struct counter *state_id_counter;
};

struct nhmm_hmm *nhmm_hmm_create(const struct nhmm_alphabet *alphabet)
{
    struct nhmm_hmm *hmm = malloc(sizeof(struct nhmm_hmm));
    hmm->alphabet = alphabet;
    hmm->state_id_counter = counter_create();
    tbl_state_create(&hmm->tbl_states);
    return hmm;
}

int nhmm_hmm_add_state(struct nhmm_hmm *hmm, const struct nhmm_state *state,
                       double start_lprob)
{
    int state_id = counter_next(hmm->state_id_counter);
    if (state_id == -1)
        return NHMM_STATE_ID_INVALID;
    tbl_state_add_state(&hmm->tbl_states, state_id, state);
    return state_id;
}

int nhmm_hmm_del_state(struct nhmm_hmm *hmm, int state_id)
{
    if (tbl_state_del_state(&hmm->tbl_states, state_id))
        return -1;

    return 0;
}

const struct nhmm_state *nhmm_hmm_get_state(const struct nhmm_hmm *hmm, int state_id)
{
    const struct nhmm_state *state = tbl_state_get_state(hmm->tbl_states, state_id);
    if (!state)
        return NULL;

    return state;
}

int nhmm_hmm_set_trans(struct nhmm_hmm *hmm, int src_state_id, int dst_state_id,
                       double lprob)
{
    struct tbl_trans **tbl_transitions =
        tbl_state_get_transitions(hmm->tbl_states, src_state_id);

    if (!tbl_transitions) {
        error("source state not found");
        return -1;
    }

    if (!tbl_state_get_state(hmm->tbl_states, dst_state_id)) {
        error("destination state not found");
        return -1;
    }

    tbl_trans_set_lprob(tbl_transitions, dst_state_id, lprob);
    return 0;
}

const struct nhmm_alphabet *nhmm_hmm_alphabet(const struct nhmm_hmm *hmm)
{
    return hmm->alphabet;
}

double nhmm_hmm_likelihood(const struct nhmm_hmm *hmm, const char *seq, const struct nhmm_path *path)
{
    struct nhmm_path *elem = NULL;
    int i = 0;
    double lprob = 0.0;
    const char *sub_seq = seq;
    DL_FOREACH(path, elem) {
        if (i == 0)
        {
            int state_id = elem->state_id;
            size_t seq_len = elem->seq_len;
            const struct nhmm_state *state = nhmm_hmm_get_state(hmm, elem->state_id);
            /* lprob = hmm_init_lprob(state_id) + nhmm_state_emission_lprob(state, sub_seq, seq_len); */

        } else {

        }
        ++i;
    }
}

void nhmm_hmm_destroy(struct nhmm_hmm *hmm)
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
