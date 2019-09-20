#include "hmm.h"
#include "alphabet.h"
#include "counter.h"
#include "state/state.h"
#include <stdlib.h>

struct nhmm_hmm *nhmm_hmm_create(const struct nhmm_alphabet *alphabet)
{
    struct nhmm_hmm *hmm = malloc(sizeof(struct nhmm_hmm));
    hmm->alphabet = alphabet;
    hmm->state_id_counter = counter_create();
    return hmm;
}

int nhmm_hmm_add_state(struct nhmm_hmm *hmm, const struct nhmm_state *state, double start_lprob) {

    int state_id = counter_next(hmm->state_id_counter);
    return state_id;
}

int nhmm_hmm_del_state(struct nhmm_hmm *hmm, int state_id){}

const struct nhmm_alphabet *nhmm_hmm_alphabet(const struct nhmm_hmm *hmm) { return hmm->alphabet; }

void nhmm_hmm_destroy(struct nhmm_hmm *hmm)
{
    if (!hmm)
        return;

    if (hmm->state_id_counter)
        counter_destroy(hmm->state_id_counter);

    hmm->alphabet = NULL;
    hmm->state_id_counter = NULL;
    free(hmm);
}
