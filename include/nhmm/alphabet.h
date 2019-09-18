#ifndef NHMM_ALPHABET_H
#define NHMM_ALPHABET_H

#include "nhmm/api.h"

struct nhmm_alphabet;

NHMM_API struct nhmm_alphabet *nhmm_create_alphabet(const char* abc);
NHMM_API void nhmm_destroy_alphabet(struct nhmm_alphabet *alphabet);

#endif
