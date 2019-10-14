#include "imm.h"
#include "src/logaddexp/logaddexp.h"
#include <math.h>

int imm_isninf(double a) { return isinf(a) && a < 0.0; }

int imm_isnan(double a) { return isnan(a); }

double imm_logsumexp(double *arr, int len)
{
    double r = LOG0;
    for (int i = 0; i < len; ++i)
        r = logaddexp(r, arr[i]);
    return r;
}

int imm_lognormalize(double *arr, int len)
{
    double lnorm = imm_logsumexp(arr, len);
    if (!isfinite(lnorm)) {
        imm_error("non-finite normalization denominator");
        return -1;
    }

    for (int i = 0; i < len; ++i)
        arr[i] -= lnorm;

    return 0;
}
