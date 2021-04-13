#include "imm/imm.h"
#include "std.h"

int imm_lprob_normalize(imm_float* arr, size_t len)
{
    imm_float lnorm = imm_lprob_sum(arr, len);
    if (!isfinite(lnorm)) {
        error("%s: non-finite normalization denominator", explain(IMM_ILLEGALARG));
        return IMM_ILLEGALARG;
    }

    for (size_t i = 0; i < len; ++i)
        arr[i] -= lnorm;

    return IMM_SUCCESS;
}

imm_float imm_lprob_sum(imm_float const* arr, size_t len)
{
    imm_float r = imm_lprob_zero();
    for (size_t i = 0; i < len; ++i)
        r = logaddexp(r, arr[i]);
    return r;
}
