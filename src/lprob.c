#include "imm/lprob.h"
#include "imm/report.h"

double imm_lprob_sum(double const* arr, size_t len)
{
    double r = imm_lprob_zero();
    for (size_t i = 0; i < len; ++i)
        r = logaddexp(r, arr[i]);
    return r;
}

int imm_lprob_normalize(double* arr, size_t len)
{
    double lnorm = imm_lprob_sum(arr, len);
    if (!isfinite(lnorm)) {
        imm_error("non-finite normalization denominator");
        return 1;
    }

    for (size_t i = 0; i < len; ++i)
        arr[i] -= lnorm;

    return 0;
}
