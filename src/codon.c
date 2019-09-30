#include "nhmm.h"
#include "report.h"
#include "stdlib.h"
#include <math.h>

#define NHMM_API_EXPORTS

struct nhmm_codon
{
    double emiss_lprobs[4 * 4 * 4];
};

NHMM_API struct nhmm_codon *nhmm_codon_create(void)
{
    return malloc(sizeof(struct nhmm_codon));
}

NHMM_API void nhmm_codon_set_lprob(struct nhmm_codon *codon, int a, int b, int c,
                                   double lprob)
{
    if (a < 0 || a > 3 || b < 0 || b > 3 || c < 0 || c > 3)
        error("base index outside the range [0, 3]");

    codon->emiss_lprobs[4 * 4 * a + 4 * b + c] = lprob;
}

NHMM_API void nhmm_codon_set_ninfs(struct nhmm_codon *codon)
{
    for (size_t i = 0; i < 4 * 4 * 4; ++i)
        codon->emiss_lprobs[i] = -INFINITY;
}

NHMM_API const double *nhmm_codon_get_lprobs(const struct nhmm_codon *codon)
{
    return codon->emiss_lprobs;
}

NHMM_API void nhmm_codon_destroy(struct nhmm_codon *codon)
{
    if (codon)
        free(codon);
}
