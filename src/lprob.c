#include "imm/imm.h"
#include "logaddexp.h"
#include <math.h>

double imm_lprob_zero(void) { return -INFINITY; }

int imm_lprob_is_zero(double a) { return isinf(a) && a < 0.0; }

int imm_lprob_is_valid(double a) { return !isnan(a); }

double imm_lprob_invalid(void) { return NAN; }

double imm_lprob_add(double a, double b) { return logaddexp(a, b); }

double imm_lprob_sum(double* arr, int len)
{

    double r = imm_lprob_zero();
    for (int i = 0; i < len; ++i)
        r = logaddexp(r, arr[i]);
    return r;
}

int imm_lprob_normalize(double* arr, int len)
{
    double lnorm = imm_lprob_sum(arr, len);
    if (!isfinite(lnorm)) {
        imm_error("non-finite normalization denominator");
        return 1;
    }

    for (int i = 0; i < len; ++i)
        arr[i] -= lnorm;

    return 0;
}
