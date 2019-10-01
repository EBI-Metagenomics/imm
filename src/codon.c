#include "array.h"
#include "nhmm.h"
#include "report.h"
#include "stdlib.h"
#include <math.h>
#include <string.h>

#define NHMM_API_EXPORTS

struct nhmm_codon
{
    double emiss_lprobs[4 * 4 * 4];
};

int codon_check_range(int a, int b, int c);

NHMM_API struct nhmm_codon *nhmm_codon_create(void)
{
    return malloc(sizeof(struct nhmm_codon));
}

NHMM_API struct nhmm_codon *nhmm_codon_clone(const struct nhmm_codon *codon)
{
    struct nhmm_codon *c = malloc(sizeof(struct nhmm_codon));
    memcpy(c->emiss_lprobs, codon->emiss_lprobs, sizeof(double) * 4 * 4 * 4);
    return c;
}

NHMM_API void nhmm_codon_set_lprob(struct nhmm_codon *codon, int a, int b, int c,
                                   double lprob)
{
    codon_check_range(a, b, c);

    if (a < 0 || a > 3 || b < 0 || b > 3 || c < 0 || c > 3)
        error("base index outside the range [0, 3]");

    codon->emiss_lprobs[4 * 4 * a + 4 * b + c] = lprob;
}

NHMM_API void nhmm_codon_set_ninfs(struct nhmm_codon *codon)
{
    for (size_t i = 0; i < 4 * 4 * 4; ++i)
        codon->emiss_lprobs[i] = -INFINITY;
}

NHMM_API double nhmm_codon_get_lprob(const struct nhmm_codon *codon, int a, int b,
                                     int c)
{
    if (codon_check_range(a, b, c))
        return NAN;

    return codon->emiss_lprobs[4 * 4 * a + 4 * b + c];
}

NHMM_API int nhmm_codon_normalize(struct nhmm_codon *codon)
{
    return log_normalize(codon->emiss_lprobs, 4 * 4 * 4);
}

NHMM_API void nhmm_codon_destroy(struct nhmm_codon *codon)
{
    if (codon)
        free(codon);
}

int codon_check_range(int a, int b, int c)
{
    if (a < 0 || a > 3 || b < 0 || b > 3 || c < 0 || c > 3) {
        error("base index outside the range [0, 3]");
        return -1;
    }
    return 0;
}
