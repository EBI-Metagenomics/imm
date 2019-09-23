#ifndef NHMM_HMM_H
#define NHMM_HMM_H

#include "nhmm/alphabet.h"
#include "nhmm/state.h"

struct nhmm_hmm;

struct nhmm_hmm *nhmm_hmm_create(const struct nhmm_alphabet *alphabet);
int nhmm_hmm_add_state(struct nhmm_hmm *hmm, const struct nhmm_state *state,
                       double start_lprob);
int nhmm_hmm_del_state(struct nhmm_hmm *hmm, int state_id);
const struct nhmm_state *nhmm_hmm_get_state(const struct nhmm_hmm *hmm, int state_id);
int nhmm_hmm_set_trans(struct nhmm_hmm *hmm, int src_state_id, int dst_state_id,
                       double lprob);
const struct nhmm_alphabet *nhmm_hmm_alphabet(const struct nhmm_hmm *hmm);
void nhmm_hmm_destroy(struct nhmm_hmm *hmm);

#endif
