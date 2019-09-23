#ifndef NHMM_STATE_H_API
#define NHMM_STATE_H_API

#include "nhmm/alphabet.h"
#include "nhmm/api.h"

#include <stdbool.h>
#include <stddef.h>

#define NHMM_STATE_ID_INVALID -1

struct nhmm_state;

NHMM_API struct nhmm_state *nhmm_state_create_normal(
    const char *name, const struct nhmm_alphabet *alphabet, double *emission_lprobs);
NHMM_API struct nhmm_state *nhmm_state_create_silent(
    const char *name, const struct nhmm_alphabet *alphabet);

NHMM_API const char *nhmm_state_name(const struct nhmm_state *state);
NHMM_API const struct nhmm_alphabet *nhmm_state_alphabet(
    const struct nhmm_state *state);
NHMM_API void nhmm_state_set_end_state(struct nhmm_state *state, bool end_state);
NHMM_API double nhmm_state_emission_lprob(const struct nhmm_state *state,
                                          const char *seq, size_t seq_len);
NHMM_API void nhmm_state_destroy(struct nhmm_state *state);

#endif
