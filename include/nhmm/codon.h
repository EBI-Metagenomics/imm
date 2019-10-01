#ifndef NHMM_CODON_H_API
#define NHMM_CODON_H_API

#include "nhmm/api.h"

struct nhmm_codon;

NHMM_API struct nhmm_codon *nhmm_codon_create(void);
NHMM_API struct nhmm_codon *nhmm_codon_clone(const struct nhmm_codon *codon);
NHMM_API void nhmm_codon_set_lprob(struct nhmm_codon *codon, int a, int b, int c,
                                   double lprob);
NHMM_API void nhmm_codon_set_ninfs(struct nhmm_codon *codon);
NHMM_API double nhmm_codon_get_lprob(const struct nhmm_codon *codon, int a, int b,
                                     int c);
NHMM_API int nhmm_codon_normalize(struct nhmm_codon *codon);
NHMM_API void nhmm_codon_destroy(struct nhmm_codon *codon);

#endif
