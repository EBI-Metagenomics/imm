#ifndef NHMM_ALPHABET_H
#define NHMM_ALPHABET_H

#include "nhmm/api.h"
#include <stddef.h>

struct nhmm_alphabet;

NHMM_API struct nhmm_alphabet *nhmm_alphabet_create(const char *symbols);
NHMM_API size_t nhmm_alphabet_length(const struct nhmm_alphabet *alphabet);
NHMM_API void nhmm_alphabet_destroy(struct nhmm_alphabet *alphabet);

#endif
