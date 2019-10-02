#define IMM_API_EXPORTS

#include "array.h"
#include "imm.h"
#include "report.h"
#include "stdlib.h"
#include <math.h>
#include <string.h>

struct imm_codon
{
    double emiss_lprobs[4 * 4 * 4];
};

int codon_check_range(int a, int b, int c);

IMM_API struct imm_codon *imm_codon_create(void)
{
    return malloc(sizeof(struct imm_codon));
}

IMM_API struct imm_codon *imm_codon_clone(const struct imm_codon *codon)
{
    struct imm_codon *c = malloc(sizeof(struct imm_codon));
    memcpy(c->emiss_lprobs, codon->emiss_lprobs, sizeof(double) * 4 * 4 * 4);
    return c;
}

IMM_API void imm_codon_set_lprob(struct imm_codon *codon, int a, int b, int c,
                                   double lprob)
{
    codon_check_range(a, b, c);

    if (a < 0 || a > 3 || b < 0 || b > 3 || c < 0 || c > 3)
        error("base index outside the range [0, 3]");

    codon->emiss_lprobs[4 * 4 * a + 4 * b + c] = lprob;
}

IMM_API void imm_codon_set_ninfs(struct imm_codon *codon)
{
    for (size_t i = 0; i < 4 * 4 * 4; ++i)
        codon->emiss_lprobs[i] = -INFINITY;
}

IMM_API double imm_codon_get_lprob(const struct imm_codon *codon, int a, int b,
                                     int c)
{
    if (codon_check_range(a, b, c))
        return NAN;

    return codon->emiss_lprobs[4 * 4 * a + 4 * b + c];
}

IMM_API int imm_codon_normalize(struct imm_codon *codon)
{
    return log_normalize(codon->emiss_lprobs, 4 * 4 * 4);
}

IMM_API void imm_codon_destroy(struct imm_codon *codon)
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
