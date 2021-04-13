#include "imm/imm.h"
#include "log.h"

int imm_lprob_normalize(imm_float* arr, size_t len)
{
    imm_float lnorm = imm_lprob_sum(arr, len);
    if (!isfinite(lnorm)) {
        error("non-finite normalization denominator");
        return 1;
    }

    for (size_t i = 0; i < len; ++i)
        arr[i] -= lnorm;

    return 0;
}

imm_float imm_lprob_sum(imm_float const* arr, size_t len)
{
    imm_float r = imm_lprob_zero();
    for (size_t i = 0; i < len; ++i)
        r = logaddexp(r, arr[i]);
    return r;
}
