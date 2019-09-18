#ifndef NHMM_STATE_H
#define NHMM_STATE_H

#include "nhmm/api.h"
#include "nhmm/alphabet.h"

struct nhmm_state;

NHMM_API struct nhmm_state *nhmm_state_create_normal(const char *name, const struct nhmm_alphabet *a);
NHMM_API const char *nhmm_state_name(struct nhmm_state *s);
NHMM_API void nhmm_state_destroy(struct nhmm_state *s);

#endif
