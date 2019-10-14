#include "imm.h"
#include "src/logaddexp/logaddexp.h"

double imm_log_sumexp(double *arr, int len)
{
    double r = LOG0;
    for (int i = 0; i < len; ++i)
        r = logaddexp(r, arr[i]);
    return r;
}

int imm_log_normalize(double *arr, int len)
{
    double lnorm = imm_log_sumexp(arr, len);
    if (!isfinite(lnorm)) {
        imm_error("non-finite normalization denominator");
        return -1;
    }

    for (int i = 0; i < len; ++i)
        arr[i] -= lnorm;

    return 0;
}
