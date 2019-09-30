#ifndef NHMM_ALPHABET_H_API
#define NHMM_ALPHABET_H_API

#include "nhmm/api.h"
#include <stddef.h>

#define NHMM_ANY_SYMBOL '*'

struct nhmm_alphabet;

NHMM_API struct nhmm_alphabet *nhmm_alphabet_create(const char *symbols);
NHMM_API size_t nhmm_alphabet_length(const struct nhmm_alphabet *alphabet);
NHMM_API void nhmm_alphabet_destroy(struct nhmm_alphabet *alphabet);

#endif
