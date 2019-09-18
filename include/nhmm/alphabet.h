#ifndef NHMM_ALPHABET_H
#define NHMM_ALPHABET_H

#include "nhmm/api.h"

struct nhmm_alphabet;

NHMM_API struct nhmm_alphabet *nhmm_alphabet_create(const char* abc);
NHMM_API int nhmm_alphabet_length(const struct nhmm_alphabet *a);
NHMM_API void nhmm_alphabet_destroy(struct nhmm_alphabet *a);

#endif
