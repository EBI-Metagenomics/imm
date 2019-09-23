#ifndef NHMM_HMM_H_API
#define NHMM_HMM_H_API

#include "nhmm/api.h"

struct nhmm_alphabet;
struct nhmm_hmm;
struct nhmm_path;
struct nhmm_state;

NHMM_API struct nhmm_hmm *nhmm_hmm_create(const struct nhmm_alphabet *alphabet);
NHMM_API int nhmm_hmm_add_state(struct nhmm_hmm *hmm, const struct nhmm_state *state,
                                double start_lprob);
NHMM_API int nhmm_hmm_del_state(struct nhmm_hmm *hmm, int state_id);
NHMM_API const struct nhmm_state *nhmm_hmm_get_state(const struct nhmm_hmm *hmm,
                                                     int state_id);
NHMM_API int nhmm_hmm_set_trans(struct nhmm_hmm *hmm, int src_state_id,
                                int dst_state_id, double lprob);
NHMM_API const struct nhmm_alphabet *nhmm_hmm_get_alphabet(const struct nhmm_hmm *hmm);
NHMM_API double nhmm_hmm_likelihood(const struct nhmm_hmm *hmm, const char *seq,
                                    const struct nhmm_path *path);
NHMM_API void nhmm_hmm_destroy(struct nhmm_hmm *hmm);

#endif
