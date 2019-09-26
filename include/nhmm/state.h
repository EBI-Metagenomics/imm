#ifndef NHMM_STATE_H_API
#define NHMM_STATE_H_API

#include "nhmm/api.h"
#include <stdbool.h>
#include <stddef.h>

#define NHMM_INVALID_STATE_ID -1

struct nhmm_alphabet;
struct nhmm_state;

NHMM_API struct nhmm_state *nhmm_state_create_normal(
    const char *name, const struct nhmm_alphabet *alphabet, double *emiss_lprobs);
NHMM_API struct nhmm_state *nhmm_state_create_silent(
    const char *name, const struct nhmm_alphabet *alphabet);
NHMM_API const char *nhmm_state_get_name(const struct nhmm_state *state);
NHMM_API const struct nhmm_alphabet *nhmm_state_get_alphabet(
    const struct nhmm_state *state);
NHMM_API void nhmm_state_set_end_state(struct nhmm_state *state, bool end_state);
NHMM_API double nhmm_state_emiss_lprob(const struct nhmm_state *state, const char *seq,
                                       size_t seq_len);
NHMM_API int nhmm_state_normalize(struct nhmm_state *state);
NHMM_API void nhmm_state_destroy(struct nhmm_state *state);

#endif
