#include "array.h"
#include "logaddexp.h"
#include "report.h"

double logsumexp(double *arr, int len)
{
    double r = -INFINITY;
    for (int i = 0; i < len; ++i)
        r = logaddexp(r, arr[i]);
    return r;
}

int log_normalize(double *arr, int len)
{
    double lnorm = logsumexp(arr, len);
    if (!isfinite(lnorm)) {
        error("non-finite normalization denominator");
        return -1;
    }

    for (int i = 0; i < len; ++i)
        arr[i] -= lnorm;

    return 0;
}
