#include "imm/lprob.h"
#include "imm/report.h"
#include "logaddexp.h"

double imm_lprob_zero(void) { return IMM_LPROB_ZERO; }

bool imm_lprob_is_zero(double a) { return isinf(a) && a < 0.0; }

bool imm_lprob_is_valid(double a) { return !isnan(a); }

double imm_lprob_invalid(void) { return IMM_LPROB_INVALID; }

double imm_lprob_add(double a, double b) { return logaddexp(a, b); }

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
